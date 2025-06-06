#pragma once

#include <algorithm>
#include <functional>
#include <memory>

#include "IWidget.h"

typedef int Color;

class ProgressBar final: public IWidget {
    using OnTouchCallback = std::function<void(double)>;
    using GetProgressCallback = std::function<double()>;

public:
    ProgressBar() = default;
    ~ProgressBar() override;
    IWidget* height(int h) override { _height = (h == -1)? (_parent != nullptr)? _parent->getHeight(): 0: h; return this; }
    IWidget* width(int w) override { _width = (w == -1)? (_parent != nullptr)? _parent->getWidth(): 0: w; return this; }
    IWidget* parent(IWidget *p) override { _parent = p; return this; }
    IWidget* marginTop(int m) override { _marginTop = m; return this; }
    IWidget* marginLeft(int m) override { _marginLeft = m; return this; }
    IWidget* onTouch(OnTouchCallback &&callback) { _onTouch = std::move(callback); return this; }
    IWidget* getProgressCb(GetProgressCallback &&callback) { _getProgressCallback = std::move(callback); return this; }
    IWidget* color(Color color) { _color = color; return this; }
    IWidget* bgColor(Color color) { _bgColor = color; return this; }
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
    OnTouchCallback& getOnTouchCallback() { return _onTouch; }
    Color getColor() { return _color; }
    Color getBgColor() { return _bgColor; }
    WidgetState getState() override { return _state; }

    void onUpdateCallback(double progress);

private:
    double _progress{0.0};
    const double _mxProgress = 1.0;
    const double _mnProgress = 0.0;

    WINDOW* _window{nullptr};

    int _height{0};
    int _width{0};
    IWidget* _parent{nullptr};
    int _marginTop{0};
    int _marginLeft{0};
    OnTouchCallback _onTouch;
    GetProgressCallback _getProgressCallback;
    Color _color;
    Color _bgColor;
    WidgetState _state{READY};
    bool _commit{true};
};