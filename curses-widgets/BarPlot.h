#pragma once

#include <memory>
#include <condition_variable>

#include "IWidget.h"

template<typename T = double>
class BarPlot final: public IWidget {
public:
    BarPlot(
        int height,
        int width,
        int nBins,
        bool hasBox = true,
        T maxY = 1.0,
        T minY = 0.0,
        T maxX = 10.0,
        T minX = 0.0
    ):
    _height(height),
    _width(width),
    _nBins(nBins),
    _maxY(maxY),
    _minY(minY),
    _maxX(maxX),
    _minX(minX),
    _commit(true),
    _box(hasBox),
    _pData(std::make_unique<T>(nBins))
    {}

    void update() override {
        for (int i = 0; i < _nBins; i++) {
            _drawBar(i, _pData[i]);
        }
    }

    void handleEvent(int key, MEVENT event) override {

    }

    WidgetState getState() const override {
        return _state;
    }

    [[nodiscard]] int height() const override{ return _height; }
    void setHeight(const int height) override { _height = height; }

    [[nodiscard]] int width() const override { return _width; }
    void setWidth(const int width) override { _width = width;}

    [[nodiscard]] int posx() const override { return _posx; }
    void setPosx(const int posx) override {
        _posx = posx;
        mvwin(_window, _parentY + _posy, _parentX + _posx);
        if (_box) box(_window, 0, 0);
    }

    [[nodiscard]] int posy() const override { return _posy; }
    void setPosy(const int posy) override {
        _posy = posy;
        mvwin(_window, _parentY + _posy, _parentX + _posx);
        if (_box) box(_window, 0, 0);
    }

    void setColor(const int color)  { if (color >= 0 && color < 8) _color = color; }
    int getColor() const { return _color; }


private:
    int _height;
    int _width;
    int _parentX;
    int _parentY;
    int _posx;
    int _posy;
    int _nBins;
    T _maxY;
    T _minY;
    T _maxX;
    T _minX;
    bool _commit;
    int _color;
    bool _box;
    std::unique_ptr<T> _pData;
    WidgetState _state;
    WINDOW* _window;

    void _drawBar(int index, T value) {

    }
};
