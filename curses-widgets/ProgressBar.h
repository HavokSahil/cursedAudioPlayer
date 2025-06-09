#pragma once

#include <algorithm>
#include <functional>
#include <memory>

#include "Widget.h"

typedef short Color;

class ProgressBar final: public Widget {
    using OnTouchCallback = std::function<void(double)>;
    using GetProgressCallback = std::function<double()>;

public:
    ProgressBar() = default;
    ~ProgressBar() override;

    Widget* onTouch(OnTouchCallback &&callback) { _onTouch = std::move(callback); return this; }
    Widget* getProgressCb(GetProgressCallback &&callback) { _getProgressCallback = std::move(callback); return this; }
    Widget* color(Color color) { _color = color; return this; }
    Widget* bgColor(Color color) { _bgColor = color; return this; }

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<Widget> child) override {}

    OnTouchCallback& getOnTouchCallback() { return _onTouch; }
    GetProgressCallback& getGetProgressCallback() { return _getProgressCallback; }

    Color getColor() { return _color; }
    Color getBgColor() { return _bgColor; }

    void onUpdateCallback(double progress);

private:
    double _progress{0.0};
    bool color_init{true};
    const double _mxProgress = 1.0;
    const double _mnProgress = 0.0;

    OnTouchCallback _onTouch{ [](double) {} };
    GetProgressCallback _getProgressCallback{ []()->double { return 0.0; } };
    Color _color{ COLOR_GREEN };
    Color _bgColor{ COLOR_BLACK };
};