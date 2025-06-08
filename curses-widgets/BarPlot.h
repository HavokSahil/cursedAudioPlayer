#pragma once
#include <cstring>
#include <functional>

#include "Widget.h"

typedef short Color;

class BarPlot final: public Widget {

    using AcquireDataCb = std::function<void(double*, size_t)>;

public:
    BarPlot() = default;
    ~BarPlot() override;

    BarPlot* minY(double m) { _minY = m; return this; }
    BarPlot* maxY(double m) { _maxY = m; return this; }
    BarPlot* nBins(int n) { _nBins = n; _datapoints = std::make_unique<double[]>(_nBins); return this; }
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
    double _minY{0.0};
    double _maxY{1.0};
    int _nBins{0};
    int _binWidth{-1};
    Color _color;
    Color _bgColor;
    std::unique_ptr<double[]> _datapoints{nullptr};
    AcquireDataCb _acquireDataCb{[](double* buff, const size_t size) { memset(buff, 0, size*sizeof(double)); }};
};
