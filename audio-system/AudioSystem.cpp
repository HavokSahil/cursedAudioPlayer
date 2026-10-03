#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include "AudioSystem.h"

#include "AudioDecoder.h"
#include "AudioPlayer.h"
#include "RingBuffer.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

AudioSystem::AudioSystem(std::string &filename, size_t bufferSize):
_filename(filename) {
    if (bufferSize < 64 || bufferSize > (1U << 20) || (bufferSize & (bufferSize - 1)) != 0)
        throw std::invalid_argument("Chunk size must be a power of two between 64 and 1048576");
    chunkSize = bufferSize;
    // Audio Decoder as default audio source
    _audioSource = std::make_unique<AudioDecoder>(filename);
    // Instantiate ring buffer with params of audio source
    _ringBuffer = std::make_unique<RingBuffer>(
            _audioSource->getChannels(),
            std::max<size_t>(8192, bufferSize * 2)
        );

    auto dataCallback = [this](float* out, const size_t frames, size_t& framesRead) {
        const Err result = _ringBuffer->read(out, frames, framesRead);
        _currentFrame += framesRead;
        const float gain = _isMute.load() ? 0.0f : _volume.load();
        for (size_t i = 0; i < framesRead * _audioSource->getChannels(); ++i)
            out[i] *= gain;
        return result;
    };

    auto queryCallback = [this]() -> size_t {
        return _ringBuffer->framesInBuffer();
    };

    // Initiate the Audio player as default audio sink for ring buffer
    _audioSink = std::make_unique<AudioPlayer>(
        _audioSource->getSampleRate(),
        _audioSource->getChannels(),
        dataCallback,
        queryCallback
    );
}

AudioSystem::~AudioSystem() {
    setIsPlaying(false);
}

size_t AudioSystem::getCurrentFrame() const {
    return _currentFrame;
}

size_t AudioSystem::getTotalFrames() const {
    return _audioSource->getTotalPcmFrames();
}

std::chrono::milliseconds AudioSystem::getElapsedTimeMs() const {
    return std::chrono::milliseconds(getCurrentFrame() * 1000 / _audioSource->getSampleRate());
}

std::chrono::milliseconds AudioSystem::getTotalDurationMs() const {
    return _audioSource->getTotalDurationMs();
}

AudioSystemInfo AudioSystem::getInfo() const {
    const double bitrate = (static_cast<double>(_audioSource->getSampleRate()) *
        static_cast<double>(_audioSource->getChannels()) *
        sizeof(float) * 8) / 1000.0;

    return AudioSystemInfo{
        _filename.c_str(),
        "Decoded PCM audio",
        "float32",
        _audioSource->getSampleRate(),
        _audioSource->getChannels(),
        _audioSource->getTotalPcmFrames(),
        bitrate,
        _audioSource->getTotalDurationMs(),
    };
}

AudioSystemState AudioSystem::getState() const {
    const size_t currentFrame = getCurrentFrame();
    const size_t elapsedMs = currentFrame * 1000  / _audioSource->getSampleRate();
    return AudioSystemState{
        _isPlaying,
        _isMute,
        _volume,
        _isLoop,
        currentFrame,
        std::chrono::milliseconds(elapsedMs),
    };
}

size_t AudioSystem::getChunkSize() const {
    return chunkSize;
}


void AudioSystem::_background_loop() {
    const size_t nChannels = _audioSource->getChannels();
    const size_t bufferSize = chunkSize * nChannels;

    const auto buffer = std::make_unique<float[]>(bufferSize);
    const auto channelBuffers = std::make_unique<std::vector<std::vector<float>>>(nChannels, std::vector<float>(chunkSize));

    while (_isPlaying && _audioSource->getState() == AudioSourceState::READY) {
        size_t framesRead;
        if (_audioSource->read(buffer.get(), chunkSize, framesRead) == ERR_OK) {
            if (framesRead == 0) break;
            const float gain = _isMute.load() ? 0.0f : _volume.load();
            for (size_t i = 0; i < framesRead; i++) {
                for (size_t c = 0; c < nChannels; c++) {
                    size_t index = i * nChannels + c;
                    channelBuffers->at(c)[i] = buffer.get()[index] * gain;
                }
            }

            for (size_t c = 0; c < nChannels; c++) {
                this->publish(channelBuffers->at(c).data(), framesRead, c);
            }

            size_t written = 0;
            while (written < framesRead && _isPlaying) {
                size_t framesWritten;
                const size_t offset = written * _audioSource->getChannels();
                const size_t remaining = framesRead - written;
                if (_ringBuffer->write(
                    buffer.get() + offset,
                    remaining,
                    framesWritten
                ) != ERR_OK) {
                    LOGE("Failed to write frames to the buffer");
                    break;
                };
                written += framesWritten;
                if (framesWritten == 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
        } else {
            LOGE("Failed to read from audio source in background loop");
            break;
        }
    }

    while (_isPlaying && !_ringBuffer->empty() && _audioSink->getState() != STOPPED)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    _audioSink->pause();
    _isPlaying = false;
}

bool AudioSystem::getIsPlaying() const {
    return _isPlaying;
}

bool AudioSystem::getIsMute() const {
    return _isMute;
}

double AudioSystem::getVolume() const {
    return _volume;
}


void AudioSystem::setIsPlaying(const bool isPlaying) {
    if (isPlaying && _isPlaying) return;
    if (!isPlaying) {
        _isPlaying = false;
        if (_thread.joinable()) _thread.join();
        _audioSink->pause();
        _audioSource->jumpToFrame(getCurrentFrame());
        _ringBuffer->reset();
        return;
    }

    if (_thread.joinable()) _thread.join();
    _audioSink->pause();
    if (_audioSource->getState() == AudioSourceState::FINISHED) reset();
    if (_audioSink->play() != ERR_OK) throw std::runtime_error("Failed to start playback");
    _isPlaying = true;
    _thread = std::thread(&AudioSystem::_background_loop, this);
}

void AudioSystem::reset() {
    setIsPlaying(false);
    _audioSource->reset();
    _currentFrame = 0;
    _ringBuffer->reset();
}

void AudioSystem::setVolume(const double volume) {
    _volume = std::isfinite(volume) ? std::clamp(volume, 0.0, 1.0) : 0.0;
}


void AudioSystem::setIsMute(const bool isMute) {
    _isMute = isMute;
}

void AudioSystem::seek(const std::chrono::milliseconds time) {
    const auto target = std::clamp(time, std::chrono::milliseconds(0), getTotalDurationMs());
    if (target == getTotalDurationMs()) seek(getTotalFrames());
    else seek(static_cast<size_t>(target.count()) * _audioSource->getSampleRate() / 1000);
}

void AudioSystem::seek(const size_t frame) {
    const bool wasPlaying = _isPlaying;
    setIsPlaying(false);
    const size_t target = std::min(frame, getTotalFrames());
    if (_audioSource->jumpToFrame(target)) _currentFrame = target;
    _ringBuffer->reset();
    if (wasPlaying && frame < getTotalFrames()) setIsPlaying(true);
}
