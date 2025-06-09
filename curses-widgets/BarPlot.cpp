#include "BarPlot.h"
#include <algorithm>
#include <cmath>
#include <mutex>
#include <atomic>

BarPlot::~BarPlot() {
    _stop();
    if (_window) delwin(_window);
}

void BarPlot::update() {
    if (!_isRunning) {
        _start();
    }
    if (!_needsRedraw) return;

    std::lock_guard<std::mutex> lock(_mutex);

    if (color_init) {
        init_pair(_id, _color, _bgColor);
        init_pair(_id + 1, _bgColor, _color);
        color_init = false;
    }

    wrefresh(_window);

    const int height = getHeight() - 1;
    const float range = _maxY - _minY;
    if (range == 0) return;

    const float midY = (_maxY + _minY) / 2.0f;
    const int offsetX = std::max((getWidth() - 2 * _nBins) / 2, 0);

    for (int i = 0; i < _nBins; ++i) {
        float value = static_cast<float>(_datapoints[i]);
        if (std::isnan(value)) value = 0.0f;

        value = std::clamp(value * 1.0, _minY, _maxY);

        int barStartY = 0, barEndY = 0;
        if (_minY * _maxY < 0.0f) {
            float zeroY = height * (_maxY - midY) / range;
            float valY  = height * (_maxY - value) / range;
            if (value >= midY) {
                barStartY = static_cast<int>(std::min(valY, zeroY));
                barEndY   = static_cast<int>(zeroY);
            } else {
                barStartY = static_cast<int>(zeroY);
                barEndY   = static_cast<int>(std::max(valY, zeroY));
            }
        } else {
            barStartY = static_cast<int>(height * (_maxY - value) / range);
            barEndY   = height;
        }

        if (barStartY == barEndY) barStartY--;

        auto& [prevStartY, prevEndY] = _prevEndpoints->at(i);
        if (prevStartY != barStartY || prevEndY != barEndY) {
            int x = offsetX + 2 * i;

            wattron(_window, COLOR_PAIR(_id));
            for (int y = prevStartY; y < prevEndY; ++y)
                mvwaddch(_window, y, x, ' ');
            wattroff(_window, COLOR_PAIR(_id));

            wattron(_window, COLOR_PAIR(_id + 1));
            for (int y = barStartY; y < barEndY; ++y)
                mvwaddch(_window, y, x, ' ');
            wattroff(_window, COLOR_PAIR(_id + 1));

            _prevEndpoints->at(i) = {barStartY, barEndY};
        }

        wattron(_window, COLOR_PAIR(_id));
        wattron(_window, A_BOLD);
        mvwprintw(_window, getHeight() - 1, offsetX, _title.c_str());
        wattroff(_window, A_BOLD);
        wattroff(_window, COLOR_PAIR(_id));
    }

    _needsRedraw = false;
}

void BarPlot::handleEvent(int ch, MEVENT &event) {
    // Placeholder for user input/event logic
}

void BarPlot::_start() {
    if (_isRunning) return;
    _isRunning = true;
    _thread = std::thread(&BarPlot::_loop, this);
}

void BarPlot::_stop() {
    _isRunning = false;
    if (_thread.joinable()) _thread.join();
}

void BarPlot::_loop() {
    while (_isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        if (!_window) continue;

        _acquireDataCb(_datapoints.get());
        _needsRedraw = true;
    }
}
