#pragma once
#include <cstring>
#include <functional>
#include <thread>

#include "Widget.h"

typedef short Color;
typedef std::vector<std::pair<int, int>> V;

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
        _prevEndpoints = std::make_unique<V>(_nBins, std::pair<int, int>(0, 0));
        return this;
    }
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
    std::atomic<bool> _isRunning{false};
    std::thread _thread;
    std::mutex _mutex;
    std::atomic<bool> _needsRedraw = false;
    bool color_init{true};
    std::string _title{"Bar Plot"};

    double _minY{0.0};
    double _maxY{1.0};
    int _nBins{0};
    int _binWidth{-1};
    Color _color;
    Color _bgColor;
    std::unique_ptr<float[]> _datapoints{nullptr};
    std::unique_ptr<V> _prevEndpoints{nullptr};
    AcquireDataCb _acquireDataCb{[&](float* buff) { memset(buff, 0, _nBins*sizeof(float)); }};

    void _start();
    void _stop();
    void _loop();
};
