#include "AudioPlayer.h"
#include <miniaudio.h>
#include <numeric>
#include <stdexcept>


AudioPlayer::AudioPlayer(const size_t sampleRate, const size_t channels, DataCallback dataCallback, QueryCallback queryCallback):
_dataCallback(std::move(dataCallback)), _queryCallback(std::move(queryCallback))
{
    _pDevice = std::make_unique<ma_device>();
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = channels;
    config.sampleRate = sampleRate;
    config.dataCallback = [](ma_device* pDevice, void* pOutput, const void*, ma_uint32 frameCount) {
        auto* self = static_cast<AudioPlayer*>(pDevice->pUserData);
        std::memset(pOutput, 0, frameCount * sizeof(float) * pDevice->playback.channels);
        size_t framesRead = 0;
        if (self->getState() == PLAYING &&
            self->_dataCallback(static_cast<float*>(pOutput), frameCount, framesRead) != ERR_OK) {
            self->setState(STOPPED);
        }
    };

    config.pUserData = this;

    if (ma_device_init(nullptr, &config, _pDevice.get()) != MA_SUCCESS) {
        LOGI("Failed to init audio device");
        throw std::runtime_error("Failed to initialize audio device");
    }

    LOGI("Device initialized");
    LOGI("Sample rate: %ld", sampleRate);
    LOGI("Channels: %ld", channels);
    _state = PAUSED;
}

AudioPlayer::~AudioPlayer() {
    ma_device_uninit(_pDevice.get());
}

AudioSinkState AudioPlayer::getState() const {
    return _state;
}

void AudioPlayer::setState(const AudioSinkState state) {
    _state = state;
}


Err AudioPlayer::play() {
    std::lock_guard lock(_mutex);
    if (_state != PAUSED) return ERR_UNKNOWN;
    LOGI("Playing...");
    _state = PLAYING;
    if (ma_device_start(_pDevice.get()) == MA_SUCCESS) {
        LOGI("Playing successfully");
        return ERR_OK;
    }
    _state = PAUSED;
    LOGI("Failed to start audio device.");
    return ERR_UNKNOWN;
}

Err AudioPlayer::pause() {
    std::lock_guard lock(_mutex);
    if (_state != PLAYING) return ERR_UNKNOWN;
    LOGI("Pausing...");
    if (ma_device_stop(_pDevice.get()) == MA_SUCCESS) {
        _state = PAUSED;
        LOGI("Paused successfully");
        return ERR_OK;
    }
    LOGI("Failed to stop audio device.");
    return ERR_UNKNOWN;
}