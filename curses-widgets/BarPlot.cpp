#include "BarPlot.h"
#include <algorithm>
#include <cmath>

BarPlot::~BarPlot() {
    if (_window) delwin(_window);
}

void BarPlot::update() {
    if (!_window || _nBins == 0) return;
    _acquireDataCb(_datapoints.get());
    if (color_init) {
        init_pair(_id, _color, _bgColor);
        color_init = false;
    }
    const int width = getmaxx(_window) - 2;
    const int height = getmaxy(_window) - 2;
    const double range = _maxY - _minY;
    if (height < 1 || width < 1 || range <= 0) return;

    werase(_window);
    wattron(_window, COLOR_PAIR(_id));
    box(_window, 0, 0);
    mvwaddnstr(_window, 0, 2, _title.c_str(), std::max(0, width - 2));
    const auto axis = _axisLabel();
    mvwaddnstr(_window, height + 1, 2, axis.c_str(), std::max(0, width - 2));
    if (_binLabels.size() == static_cast<size_t>(_nBins)) {
        for (int bin = 0; bin < _nBins; ++bin) {
            const int start = bin * width / _nBins;
            const int end = (bin + 1) * width / _nBins;
            const auto& label = _binLabels[bin];
            mvwaddnstr(_window, height + 1, start + 1, label.c_str(), end - start);
        }
    }
    const bool bipolar = _minY < 0 && _maxY > 0;
    const int zeroY = bipolar
        ? std::clamp(static_cast<int>(height * _maxY / range), 0, height - 1) : height - 1;
    for (int bin = 0; bin < _nBins; ++bin) {
        const double target = std::isfinite(_datapoints[bin])
            ? std::clamp(static_cast<double>(_datapoints[bin]), _minY, _maxY) : _minY;
        _displaypoints[bin] += _smoothing * (target - _displaypoints[bin]);
    }
    for (int x = 0; x < width; ++x) {
        const int first = x * _nBins / width;
        const int last = std::min(_nBins, std::max(first + 1, (x + 1) * _nBins / width));
        double value = _displaypoints[first];
        for (int bin = first + 1; bin < last; ++bin) value = std::max(value, static_cast<double>(_displaypoints[bin]));
        const int valueY = std::clamp(static_cast<int>(height * (_maxY - value) / range), 0, height - 1);
        if (bipolar) mvwaddch(_window, zeroY + 1, x + 1, ACS_HLINE | A_DIM);
        if ((bipolar && std::abs(value) < 0.001) || (!bipolar && valueY == zeroY)) continue;
        for (int y = std::min(zeroY, valueY); y <= std::max(zeroY, valueY); ++y)
            mvwaddch(_window, y + 1, x + 1, ACS_CKBOARD | A_BOLD);
    }
    wattroff(_window, COLOR_PAIR(_id));
    wnoutrefresh(_window);
}

void BarPlot::handleEvent(int, MEVENT&) {}
