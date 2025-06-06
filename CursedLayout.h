#pragma once
#include <csignal>
#include <functional>
#include <memory>
#include <iostream>

#include "IWidget.h"
#include "MainWindow.h"

class CursedLayout {
public:
    using SetTimeCallback = std::function<void(double)>;
    using SetPlaybackCallback = std::function<void(bool)>;
    using GetTimeCallback = std::function<double()>;
    using GetPlaybackCallback = std::function<bool()>;

    CursedLayout(
        SetTimeCallback&& setTime,
        SetPlaybackCallback&& setPlayback,
        GetTimeCallback&& getTime,
        GetPlaybackCallback&& getPlayback
    )
        : setTimeCallback(std::move(setTime)),
          setPlaybackCallback(std::move(setPlayback)),
          getTimeCallback(std::move(getTime)),
          getPlaybackCallback(std::move(getPlayback))
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
    SetPlaybackCallback setPlaybackCallback;
    GetTimeCallback getTimeCallback;
    GetPlaybackCallback getPlaybackCallback;

    // === Static handler stuff ===
    static CursedLayout* instance;

    static void handle_resize(int sig) {
        if (instance) {
            instance->resize();
        }
    }
};
