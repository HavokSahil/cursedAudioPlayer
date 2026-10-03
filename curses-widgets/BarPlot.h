#pragma once
#include <cstring>
#include <algorithm>
#include <functional>

#include "Widget.h"

typedef short Color;

class BarPlot final: public Widget {

    using AcquireDataCb = std::function<void(float*)>;

public:
    BarPlot() = default;
    ~BarPlot() override;

    BarPlot* title(const char* s) { _title = s; return this; }
    BarPlot* minY(double m) { _minY = m; return this; }
    BarPlot* maxY(double m) { _maxY = m; return this; }
    BarPlot* nBins(int n) {
        _nBins = n;
        _datapoints = std::make_unique<float[]>(_nBins);
        _displaypoints = std::make_unique<float[]>(_nBins);
        std::fill_n(_displaypoints.get(), _nBins, _minY < 0 && _maxY > 0 ? 0.0f : static_cast<float>(_minY));
        return this;
    }
    BarPlot* smoothing(double value) { _smoothing = std::clamp(value, 0.0, 1.0); return this; }
    BarPlot* binLabels(std::vector<std::string> labels) { _binLabels = std::move(labels); return this; }
    BarPlot* axisLabel(std::function<std::string()> cb) { _axisLabel = std::move(cb); return this; }
    BarPlot* binWidth(int m) { _binWidth = m; return this; }
    BarPlot* color(Color c) { _color = c; return this; }
    BarPlot* bgColor(Color c) { _bgColor = c; return this; }
    BarPlot* acquireDataCb(AcquireDataCb&& callback) { _acquireDataCb = std::move(callback); return this; }

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;

    double getMinY() { return _minY; }
    double getMaxY() { return _maxY; }
    double getNBins() { return _nBins; }
    double getBinWidth() { return _binWidth; }
    Color getColor() { return _color; }
    Color getBgColor() { return _bgColor; }

private:
    bool color_init{true};
    std::string _title{"Bar Plot"};

    double _minY{0.0};
    double _maxY{1.0};
    int _nBins{0};
    int _binWidth{-1};
    Color _color{COLOR_GREEN};
    Color _bgColor{COLOR_BLACK};
    std::unique_ptr<float[]> _datapoints{nullptr};
    std::vector<std::string> _binLabels;
    double _smoothing{1.0};
    std::unique_ptr<float[]> _displaypoints{nullptr};
    std::function<std::string()> _axisLabel{[]() { return std::string(); }};
    AcquireDataCb _acquireDataCb{[&](float* buff) { memset(buff, 0, _nBins*sizeof(float)); }};

};
