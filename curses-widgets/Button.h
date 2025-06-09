#pragma once

#include <functional>
#include <string>

#include "Widget.h"

typedef short Color;

class Button final: public Widget {
    using ButtonCallback = std::function<void(bool)>;
    using GetButtonStatusCb = std::function<bool()>;
public:
    Button() = default;
    ~Button() override;

    Widget* active(bool a) { _active = a; return this; }
    Widget* activeText(const char* c) { _activeText = c; return this; }
    Widget* inactiveText(const char* c) { _inactiveText = c; return this; }
    Widget* onClick(ButtonCallback &&callback) { _callback = std::move(callback); return this; }
    Widget* getStatusCb(GetButtonStatusCb &&callback) { _getStatusCb = std::move(callback); return this; }
    Widget* color(Color c) { _color = c; return this; }
    Widget* bgColor(Color c) { _bgColor = c; return this; }

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<Widget> child) override {}

private:
    bool _active{false};
    std::string _activeText;
    std::string _inactiveText;
    ButtonCallback _callback{[&](bool) { _active = !_active; }};
    GetButtonStatusCb _getStatusCb{[&]()->bool { return _active; }};
    Color _color{COLOR_GREEN};
    Color _bgColor{COLOR_BLACK};
};