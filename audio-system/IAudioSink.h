#pragma once
#include "err_codes.h"
#include <functional>

/// @enum AudioSinkState
/// @brief Describes the states the sink can be in
enum AudioSinkState {
    PLAYING     = 0,
    PAUSED      = 1,
    STOPPED     = 2,
    BUFFERING   = 3
};

class IAudioSink {
public:
    virtual ~IAudioSink() = default;

    virtual Err play() = 0;
    virtual Err pause() = 0;
    virtual void setState(AudioSinkState state) = 0;
    virtual AudioSinkState getState() const = 0;
};
