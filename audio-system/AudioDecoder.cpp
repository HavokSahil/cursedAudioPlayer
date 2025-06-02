#include "AudioDecoder.h"
#include <err_codes.h>
#include <miniaudio.h>

AudioDecoder::AudioDecoder(const std::string& filename):
    _state(READY)
{
    ma_decoder_config decoderConfig = ma_decoder_config_init_default();
    decoderConfig.format = ma_format_f32;
    _decoder = std::make_unique<ma_decoder>();
    if (ma_decoder_init_file(filename.c_str(), &decoderConfig, _decoder.get()) != MA_SUCCESS) {
        LOGE("Failed to init decoder");
        _state = FINISHED;
    }
    ma_uint64 total_frames = 0;

    if (ma_decoder_get_length_in_pcm_frames(_decoder.get(), &total_frames) != MA_SUCCESS) {
        LOGE("Failed to obtain total frame length");
        _state = FINISHED;
    };

    _totalFrames = total_frames;
}

AudioDecoder::~AudioDecoder() {
    ma_decoder_uninit(_decoder.get());
}

Err AudioDecoder::read(float *out, const size_t frameCount, size_t &framesRead) {
    ma_uint64 frames_read;
    if (_decoder->readPointerInPCMFrames == _totalFrames) {
        framesRead = 0;
        return ERR_OK;
    }
    if (ma_decoder_read_pcm_frames(_decoder.get(), out, frameCount, &frames_read) != MA_SUCCESS) {
        framesRead = 0;
        return ERR_UNKNOWN;
    }
    if (getCurrentPcmFrame() == getTotalPcmFrames()) {
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
    const uint32_t milliseconds_u = 1000.0 * totalFrames / sampleRate;
    return std::chrono::milliseconds(milliseconds_u);
}

bool AudioDecoder::jumpToTime(const std::chrono::milliseconds time) {
    const auto sampleRate = static_cast<double>(getSampleRate());
    const auto milliseconds_d = static_cast<double>(time.count());
    const auto frame_d = milliseconds_d * sampleRate / 1000.0 ;
    const auto frame = static_cast<size_t>(frame_d);
    if (frame > _totalFrames) return false;
    if (ma_decoder_seek_to_pcm_frame(_decoder.get(), frame) != MA_SUCCESS) {
        return false;
    }
    return true;
}

bool AudioDecoder::jumpToFrame(const size_t frame) {
    if (frame > _totalFrames) return false;
    if (ma_decoder_seek_to_pcm_frame(_decoder.get(), frame) != MA_SUCCESS) {
        return false;
    }
    return true;
}

void AudioDecoder::reset() {
    if (ma_decoder_seek_to_pcm_frame(_decoder.get(), 0)!= MA_SUCCESS) {
        _state = FINISHED;
    } else _state = READY;
}


std::chrono::milliseconds AudioDecoder::getElapsedMs() const {
    const auto currentFrame = static_cast<double>(getCurrentPcmFrame());
    const auto sampleRate = static_cast<double>(getSampleRate());
    const uint32_t milliseconds_u = 1000.0 * currentFrame / sampleRate;
    return std::chrono::milliseconds(milliseconds_u);
}

size_t AudioDecoder::getCurrentPcmFrame() const {
    return _decoder->readPointerInPCMFrames;
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