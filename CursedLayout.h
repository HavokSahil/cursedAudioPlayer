#pragma once
#include <cstdint>
#include <string>
#include <deque>
#include <functional>
#include <memory>

#include "AudioSystem.h"
#include "MainWindow.h"

class CursedLayout {
public:
    using SetTimeCallback = std::function<void(double)>;
    using GetTimeCallback = std::function<double()>;

    using SetPlaybackCallback = std::function<void(bool)>;
    using GetPlaybackCallback = std::function<bool()>;

    using SkipSecondsCallback = std::function<void(int)>;

    using SetVolumeCallback = std::function<void(double)>;
    using GetVolumeCallback = std::function<double()>;

    using GetElapsedSecString = std::function<std::string()>;
    using GetTotalSecString = std::function<std::string()>;

    using SetMuteCallback = std::function<void(bool)>;
    using GetMuteCallback = std::function<bool()>;

    using AcquireChannelDataCallback = std::function<void(float*)>;
    using GetAudioSystemInfo = std::function<AudioSystemInfo()>;

    using HexDataCallback = std::function<std::deque<std::pair<size_t, uint32_t>>()>;

    CursedLayout(
        SetTimeCallback&& setTime,
        SetPlaybackCallback&& setPlayback,
        GetTimeCallback&& getTime,
        GetPlaybackCallback&& getPlayback,
        SkipSecondsCallback&& skipPrevS,
        SkipSecondsCallback&& skipNextS,
        SetVolumeCallback&& setVolume,
        GetVolumeCallback&& getVolume,
        SetMuteCallback&& setMute,
        GetMuteCallback&& getMute,
        GetElapsedSecString&& getElapsedSecString,
        GetTotalSecString&& getTotalSecString,
        AcquireChannelDataCallback&& acquireChannelData,
        AcquireChannelDataCallback&& acquireSpecData,
        AcquireChannelDataCallback&& acquireCqtData,
        GetAudioSystemInfo&& getAudioSystemInfo,
        HexDataCallback&& hexData
    ) :
    setTimeCallback(std::move(setTime)),
    getTimeCallback(std::move(getTime)),
    setPlaybackCallback(std::move(setPlayback)),
    getPlaybackCallback(std::move(getPlayback)),
    prevSecCallback(skipPrevS),
    nextSecCallback(skipNextS),
    setVolumeCallback(setVolume),
    getVolumeCallback(getVolume),
    setMuteCallback(setMute),
    getMuteCallback(getMute),
    getElapsedSecString(getElapsedSecString),
    getTotalSecString(getTotalSecString),
    acquireChannelDataCallback(acquireChannelData),
    acquireSpecDataCallback(acquireSpecData),
    acquireCqtDataCallback(acquireCqtData),
    getAudioSystemInfo(getAudioSystemInfo),
    hexDataCallback(hexData)
    {
    }

    ~CursedLayout() = default;

    void resize();
    void mount();
    void run();

private:
    std::unique_ptr<MainWindow> mainWindow;

    SetTimeCallback setTimeCallback;
    GetTimeCallback getTimeCallback;

    SetPlaybackCallback setPlaybackCallback;
    GetPlaybackCallback getPlaybackCallback;

    SkipSecondsCallback prevSecCallback;
    SkipSecondsCallback nextSecCallback;

    SetVolumeCallback setVolumeCallback;
    GetVolumeCallback getVolumeCallback;

    SetMuteCallback setMuteCallback;
    GetMuteCallback getMuteCallback;

    GetElapsedSecString getElapsedSecString;
    GetTotalSecString getTotalSecString;

    AcquireChannelDataCallback acquireChannelDataCallback;
    AcquireChannelDataCallback acquireSpecDataCallback;
    AcquireChannelDataCallback acquireCqtDataCallback;

    GetAudioSystemInfo getAudioSystemInfo;

    HexDataCallback hexDataCallback;

};
