#pragma once

#include <chrono>
#include "err_codes.h"

/// @enum AudioSourceState
/// @brief The state of the audio source.
enum AudioSourceState {
    READY = 0,
    FINISHED = 1,
};

/// @interface IAudioSource
/// @brief The interface for audio source
class IAudioSource {
public:
    virtual ~IAudioSource() = default;
    virtual Err read(float* out, size_t frameCount, size_t &framesRead) = 0;
    /// @param time The timestamp to jump to in the track.
    /// @returns True if the jump was successful.
    virtual bool jumpToTime(std::chrono::milliseconds time) = 0;
    /// @param frame The frame to jump to in the track.
    /// @returns True if the jump was successful.
    virtual bool jumpToFrame(size_t frame) = 0;
    /// @brief Resets the audio source
    virtual void reset() = 0;
    /// @returns Current state of the audio source.
    virtual AudioSourceState getState() = 0;
    /// @returns Number of channels in the audio track.
    virtual size_t getChannels() const = 0;
    /// @returns The sample rate of the audio track.
    virtual size_t getSampleRate() const = 0;
    /// @returns Total PCM frames count of the audio track.
    virtual size_t getTotalPcmFrames() const = 0;
    /// @returns Current PCM frame index of the audio track.
    virtual size_t getCurrentPcmFrame() const = 0;
    /// @returns Total duration in milliseconds.
    virtual std::chrono::milliseconds getTotalDurationMs() const = 0;
    /// @returns Duration elapsed in milliseconds.
    virtual std::chrono::milliseconds getElapsedMs() const = 0;
};