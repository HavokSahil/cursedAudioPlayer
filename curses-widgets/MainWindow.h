#pragma once

#include <memory>
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

    void run();
    bool isRunning();
private:
    MEVENT _event{};
    bool _running = false;

    std::vector<std::shared_ptr<Widget>> _widgets;
    uint64_t _timeoutMs{50};
    WidgetState _state{READY};
    bool _commit{true};
};
