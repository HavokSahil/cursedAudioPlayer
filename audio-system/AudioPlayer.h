#pragma once

#include <memory>
#include <err_codes.h>
#include <utility>
#include <cstring>

#include "IAudioSink.h"

struct ma_device;
class RingBuffer;

/// @class AudioPlayer
/// @implements IAudioSink
/// @brief The audio player class managing the audio device
class AudioPlayer final: public IAudioSink {
public:
    using DataCallback = std::function<Err(float*, size_t, size_t&)>;
    using QueryCallback = std::function<size_t()>;
    /// @param sampleRate: The sample rate of the source data
    /// @param channels: Number of channels in source data
    /// @param dataCallback: Callback for data acquisition
    /// @param queryCallback: Callback for querying the availability of frames
    AudioPlayer(size_t sampleRate, size_t channels, DataCallback dataCallback, QueryCallback queryCallback);
    ~AudioPlayer() override;

    /// @returns The current state of the audio player
    AudioSinkState getState() const override;
    /// @param state: The state to be set
    void setState(AudioSinkState state) override;
    /// @brief Start the audio player
    Err play() override;
    /// @brief Stop the audio player
    Err pause() override;

private:
    ///@brief The state of the audio player
    AudioSinkState _state = STOPPED;
    /// @brief Callback for data acquisition
    DataCallback _dataCallback;
    /// @brief Callback for getting infos on frames availability
    QueryCallback _queryCallback;
    /// @brief miniaudio device struct pointer
    std::unique_ptr<ma_device> _pDevice;
};
