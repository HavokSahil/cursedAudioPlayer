#include "AudioPlayer.h"
#include <miniaudio.h>
#include <numeric>

static int counter = 0;

AudioPlayer::AudioPlayer(const size_t sampleRate, const size_t channels, DataCallback dataCallback, QueryCallback queryCallback):
_dataCallback(std::move(dataCallback)), _queryCallback(std::move(queryCallback))
{
    _pDevice = std::make_unique<ma_device>();
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = channels;
    config.sampleRate = sampleRate;
    config.dataCallback = [](ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        auto* self = static_cast<AudioPlayer*>(pDevice->pUserData);
        size_t framesRead;
        if (self->getState() == PLAYING && self->_queryCallback() < frameCount)
            self->setState(BUFFERING);      // Switch to buffering state when the number of frames in buffer is low
        if (self->getState() == BUFFERING && self->_queryCallback() >= frameCount)
            self->setState(PLAYING);        // Switch to playing mode, when there is enough frame in buffer
        if (self->getState() == BUFFERING) {
            std::memset(pOutput, 0, frameCount * sizeof(float) * pDevice->playback.channels);
        } else if (self->_dataCallback(static_cast<float*>(pOutput), frameCount, framesRead) != MA_SUCCESS) {
            self->setState(STOPPED);        // Stop the player in case of error
        }
    };

    config.pUserData = this;

    if (ma_device_init(nullptr, &config, _pDevice.get()) != MA_SUCCESS) {
        LOGI("Failed to init audio device");
        return;
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
    if (_state != PAUSED) return ERR_UNKNOWN;
    LOGI("Playing...");
    if (ma_device_start(_pDevice.get()) == MA_SUCCESS) {
        _state = PLAYING;
        LOGI("Playing successfully");
        return ERR_OK;
    }
    LOGI("Failed to start audio device.");
    return ERR_UNKNOWN;
}

Err AudioPlayer::pause() {
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