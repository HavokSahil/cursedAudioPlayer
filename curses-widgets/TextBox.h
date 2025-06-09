#pragma once

#include <functional>
#include <string>

#include "Widget.h"

typedef short Color;

class TextBox final: public Widget {
    using GetTextCb = std::function<std::string()>;
public:
    TextBox() = default;
    ~TextBox() override;

    Widget* text(const char* c) { _text = c; return this; }
    Widget* getTextCb(GetTextCb &&callback) { _getTextCb = std::move(callback); return this; }
    Widget* color(Color c) { _color = c; return this; }
    Widget* bgColor(Color c) { _bgColor = c; return this; }

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;

private:
    bool _selected{false};
    int _strOffset{0};
    bool _active{false};
    bool color_init{true};
    std::string _text;
    GetTextCb _getTextCb{[&]()->std::string { return _text; }};
    Color _color{COLOR_GREEN};
    Color _bgColor{COLOR_BLACK};
};