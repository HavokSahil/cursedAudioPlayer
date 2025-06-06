#pragma once

#include <functional>
#include <string>

#include "Colors.h"
#include "IWidget.h"

typedef int Color;

class Button final: public IWidget {
    using ButtonCallback = std::function<void(bool)>;
    using GetButtonStatusCb = std::function<bool()>;
public:
    Button() = default;
    ~Button() override;
    IWidget* height(int h) override { _height = (h == -1)? (_parent != nullptr)? _parent->getHeight(): 0: h; return this; }
    IWidget* width(int w) override { _width = (w == -1)? (_parent != nullptr)? _parent->getWidth(): 0: w; return this; }
    IWidget* parent(IWidget *p) override { _parent = p; return this; }
    IWidget* marginTop(int m) override { _marginTop = m; return this; }
    IWidget* marginLeft(int m) override { _marginLeft = m; return this; }
    IWidget* active(bool a) { _active = a; return this; }
    IWidget* activeText(const char* c) { _activeText = c; return this; }
    IWidget* inactiveText(const char* c) { _inactiveText = c; return this; }
    IWidget* onClick(ButtonCallback &&callback) { _callback = std::move(callback); return this; }
    IWidget* getStatusCb(GetButtonStatusCb &&callback) { _getStatusCb = std::move(callback); return this; }
    IWidget* color(Color c) { _color = c; return this; }
    IWidget* bgColor(Color c) { _bgColor = c; return this; }
    IWidget* build() override;

    void setState(WidgetState s) override { _state = s; }
    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<IWidget> child) override {}

    int getHeight() override { return _height; }
    int getWidth() override { return _width; }
    IWidget* getParent() override { return _parent; }
    int getMarginTop() override { return _marginTop; }
    int getMarginLeft() override { return _marginLeft; }
    int getTopLeftY() override;
    int getTopLeftX() override;
    WidgetState getState() override { return _state; }

private:
    WINDOW* _window{nullptr};

    int _height{0};
    int _width{0};
    IWidget* _parent{nullptr};
    int _marginTop{0};
    int _marginLeft{0};
    bool _active{false};
    std::string _activeText{""};
    std::string _inactiveText{""};
    ButtonCallback _callback{nullptr};
    GetButtonStatusCb _getStatusCb{nullptr};
    Color _color{COLOR_GREEN};
    Color _bgColor{COLOR_BLACK};
    WidgetState _state{READY};
    bool _commit{true};
};