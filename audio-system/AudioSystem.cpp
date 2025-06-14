#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include "AudioSystem.h"

#include "AudioDecoder.h"
#include "AudioPlayer.h"
#include "RingBuffer.h"

AudioSystem::AudioSystem(std::string &filename, size_t bufferSize):
_filename(filename) {
    // Audio Decoder as default audio source
    _audioSource = std::make_unique<AudioDecoder>(filename);
    // Instantiate ring buffer with params of audio source
    _ringBuffer = std::make_unique<RingBuffer>(
            _audioSource->getChannels(),
            bufferSize
        );

    auto dataCallback = [this](float* out, const size_t frames, size_t& framesRead) {
        return this->_ringBuffer->read(out, frames, framesRead);
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
    return _audioSource->getCurrentPcmFrame();
}

size_t AudioSystem::getTotalFrames() const {
    return _audioSource->getTotalPcmFrames();
}

std::chrono::milliseconds AudioSystem::getElapsedTimeMs() const {
    return _audioSource->getElapsedMs();
}

std::chrono::milliseconds AudioSystem::getTotalDurationMs() const {
    return _audioSource->getTotalDurationMs();
}

AudioSystemInfo AudioSystem::getInfo() const {
    const double bitrate = (static_cast<double>(_audioSource->getSampleRate()) *
        static_cast<double>(_audioSource->getChannels()) *
        sizeof(float)) / 1024.0;

    return (AudioSystemInfo){
        _filename.c_str(),
        "Description of file comes here",
        "float32",
        _audioSource->getSampleRate(),
        _audioSource->getChannels(),
        _audioSource->getTotalPcmFrames(),
        bitrate,
        _audioSource->getTotalDurationMs(),
    };
}

AudioSystemState AudioSystem::getState() const {
    const size_t currentFrame  = _audioSource->getCurrentPcmFrame() - _ringBuffer->framesInBuffer();
    const size_t elapsedMs = currentFrame * 1000  / _audioSource->getSampleRate();
    return (AudioSystemState){
        _isPlaying,
        _isMute,
        _volume,
        _isLoop,
        _speed,
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
            for (int i = 0; i < framesRead; i++) {
                for (int c = 0; c < nChannels; c++) {
                    size_t index = i * nChannels + c;
                    buffer.get()[index] *= _isMute? 0.0: _volume; // apply the volume filter
                    channelBuffers->at(c)[i] = buffer.get()[index];
                }
            }

            for (int c = 0; c < nChannels; c++) {
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
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        } else {
            LOGE("Failed to read from audio source in background loop");
            break;
        }
    }

    _isPlaying = false;
    _audioSink->pause();
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

double AudioSystem::getSpeed() const {
    return _speed;
}

void AudioSystem::setIsPlaying(const bool isPlaying) {
    _isPlaying = isPlaying;
    if (_isPlaying) {
        if (_audioSource->getState() == AudioSourceState::FINISHED) {
            reset();
        }
        _thread = std::thread(&AudioSystem::_background_loop, this);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        _audioSink->play();
    } else {
        _audioSink->pause();
        if (_thread.joinable()) {
            _thread.join();
        }
    }
}

void AudioSystem::reset() {
    if (_thread.joinable()) _thread.join();
    _audioSource->reset();
    _ringBuffer->reset();
}

void AudioSystem::setVolume(const double volume) {
    _volume = volume;
}

void AudioSystem::setSpeed(const double speed) {
    _speed = speed;
}

void AudioSystem::setIsMute(const bool isMute) {
    _isMute = isMute;
}

void AudioSystem::seek(const std::chrono::milliseconds time) {
    // const bool __isPlaying = _isPlaying;
    // _isPlaying = false;
    if (_isPlaying && _audioSource->getState() == AudioSourceState::READY) {
        setIsPlaying(false);
        _audioSource->jumpToTime(time);
        _ringBuffer->reset();
        setIsPlaying(true);
    } else {
        _audioSource->jumpToTime(time);
        _ringBuffer->reset();
    }
    // setIsPlaying(__isPlaying);
}

void AudioSystem::seek(const size_t frame) {
    // const bool __isPlaying = _isPlaying;
    // _isPlaying = false;
    if (_isPlaying && _audioSource->getState() == AudioSourceState::READY) {
        setIsPlaying(false);
        _audioSource->jumpToFrame(frame);
        _ringBuffer->reset();
        setIsPlaying(true);
    } else {
        _audioSource->jumpToFrame(frame);
        _ringBuffer->reset();
    }
    // setIsPlaying(__isPlaying);
}
