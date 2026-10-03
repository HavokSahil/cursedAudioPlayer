#pragma once

#include <memory>
#include <functional>
#include <cstdint>
#include <vector>

#include "Widget.h"

class MainWindow final: public Widget {
public:
    MainWindow();
    ~MainWindow() override;

    Widget* timeoutMs(int ms);

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<Widget> child) override;

    void keyCb(std::function<void(int)> cb) { _keyCallback = std::move(cb); }
    void run();
    bool isRunning();
private:
    std::function<void(int)> _keyCallback{[](int) {}};
    MEVENT _event{};
    bool _running = false;

    int _timeoutMs{16};
};
