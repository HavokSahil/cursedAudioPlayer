#pragma once
#include <csignal>
#include <functional>
#include <memory>
#include <iostream>

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


    CursedLayout(
        SetTimeCallback&& setTime,
        SetPlaybackCallback&& setPlayback,
        GetTimeCallback&& getTime,
        GetPlaybackCallback&& getPlayback,
        SkipSecondsCallback&& skipPrevS,
        SkipSecondsCallback&& skipNextS,
        SetVolumeCallback&& setVolume,
        GetVolumeCallback&& getVolume,
        GetElapsedSecString&& getElapsedSecString,
        GetTotalSecString&& getTotalSecString
    ) :
    setTimeCallback(std::move(setTime)),
    getTimeCallback(std::move(getTime)),
    setPlaybackCallback(std::move(setPlayback)),
    getPlaybackCallback(std::move(getPlayback)),
    prevSecCallback(skipPrevS),
    nextSecCallback(skipNextS),
    setVolumeCallback(setVolume),
    getVolumeCallback(getVolume),
    getElapsedSecString(getElapsedSecString),
    getTotalSecString(getTotalSecString)
    {
        instance = this;  // Set static pointer
        signal(SIGWINCH, handle_resize);  // Register handler
        mainWindow = std::make_unique<MainWindow>();
    }

    ~CursedLayout() {
        if (instance == this) instance = nullptr;
    }

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

    GetElapsedSecString getElapsedSecString;
    GetTotalSecString getTotalSecString;

    // === Static handler stuff ===
    static CursedLayout* instance;

    static void handle_resize(int sig) {
        if (instance) {
            instance->resize();
        }
    }
};
