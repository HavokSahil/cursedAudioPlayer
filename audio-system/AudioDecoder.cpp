#include "AudioDecoder.h"
#include <err_codes.h>
#include <miniaudio.h>
#include <stdexcept>

AudioDecoder::AudioDecoder(const std::string& filename):
    _filename(filename),
    _state(READY)
{
    ma_decoder_config decoderConfig = ma_decoder_config_init_default();
    decoderConfig.format = ma_format_f32;
    _decoder = std::make_unique<ma_decoder>();
    if (ma_decoder_init_file(_filename.c_str(), &decoderConfig, _decoder.get()) != MA_SUCCESS) {
        LOGE("Failed to init decoder");
        throw std::runtime_error("Failed to decode: " + filename);
    }
    ma_uint64 total_frames = 0;

    if (ma_decoder_get_length_in_pcm_frames(_decoder.get(), &total_frames) != MA_SUCCESS) {
        LOGE("Failed to obtain total frame length");
        ma_decoder_uninit(_decoder.get());
        throw std::runtime_error("Failed to read audio length: " + filename);
    };

    _totalFrames = total_frames;
}

AudioDecoder::~AudioDecoder() {
    ma_decoder_uninit(_decoder.get());
}

Err AudioDecoder::read(float *out, const size_t frameCount, size_t &framesRead) {
    ma_uint64 frames_read = 0;
    if (_decoder->readPointerInPCMFrames == _totalFrames) {
        framesRead = 0;
        return ERR_OK;
    }
    const auto result = ma_decoder_read_pcm_frames(_decoder.get(), out, frameCount, &frames_read);
    if (result != MA_SUCCESS && result != MA_AT_END) {
        framesRead = 0;
        return ERR_UNKNOWN;
    }
    _currentFrame = _decoder->readPointerInPCMFrames;
    if (result == MA_AT_END || getCurrentPcmFrame() == getTotalPcmFrames()) {
        _state = FINISHED;
    }
    framesRead = frames_read;
    return ERR_OK;
}

size_t AudioDecoder::getTotalPcmFrames() const {
    return _totalFrames;
}

std::chrono::milliseconds AudioDecoder::getTotalDurationMs() const {
    const auto sampleRate = static_cast<double>(getSampleRate());
    const auto totalFrames = static_cast<double>(getTotalPcmFrames());
    const int64_t milliseconds_u = 1000.0 * totalFrames / sampleRate;
    return std::chrono::milliseconds(milliseconds_u);
}

bool AudioDecoder::jumpToTime(const std::chrono::milliseconds time) {
    if (time.count() < 0) return false;
    const auto sampleRate = static_cast<double>(getSampleRate());
    const auto milliseconds_d = static_cast<double>(time.count());
    const auto frame_d = milliseconds_d * sampleRate / 1000.0 ;
    const auto frame = static_cast<size_t>(frame_d);
    if (frame > _totalFrames) return false;
    if (ma_decoder_seek_to_pcm_frame(_decoder.get(), frame) != MA_SUCCESS) {
        return false;
    }
    _currentFrame = frame;
    _state = frame < _totalFrames ? READY : FINISHED;
    return true;
}

bool AudioDecoder::jumpToFrame(const size_t frame) {
    if (frame > _totalFrames) return false;
    if (ma_decoder_seek_to_pcm_frame(_decoder.get(), frame) != MA_SUCCESS) {
        return false;
    }
    _currentFrame = frame;
    _state = frame < _totalFrames ? READY : FINISHED;
    return true;
}

void AudioDecoder::reset() {
    if (ma_decoder_seek_to_pcm_frame(_decoder.get(), 0)!= MA_SUCCESS) {
        _state = FINISHED;
    } else {
        _currentFrame = 0;
        _state = READY;
    };
}

std::chrono::milliseconds AudioDecoder::getElapsedMs() const {
    const auto currentFrame = static_cast<double>(getCurrentPcmFrame());
    const auto sampleRate = static_cast<double>(getSampleRate());
    const int64_t milliseconds_u = 1000.0 * currentFrame / sampleRate;
    return std::chrono::milliseconds(milliseconds_u);
}

size_t AudioDecoder::getCurrentPcmFrame() const {
    return _currentFrame;
}

size_t AudioDecoder::getChannels() const {
    return _decoder->outputChannels;
}

size_t AudioDecoder::getSampleRate() const {
    return _decoder->outputSampleRate;
}

AudioSourceState AudioDecoder::getState() {
    return _state;
}