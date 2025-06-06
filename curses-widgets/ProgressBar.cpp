#include "ProgressBar.h"
#include <algorithm>

ProgressBar::~ProgressBar() {
    if (_window) delwin(_window);
}

IWidget* ProgressBar::build() {
    if (_window == nullptr)
        _window = newwin(_height, _width, getTopLeftY(), getTopLeftX());
    else {
        mvwin(_window, getTopLeftY(), getTopLeftX());
    }
    return this;
}

void ProgressBar::update() {
    double prog = _getProgressCallback();
    if (prog != _progress) { _progress = prog; _commit = true; }
    if (_commit) {
        init_pair(1, _color, _bgColor);
        wattron(_window, COLOR_PAIR(1));
        for (int i = 0; i < _height; i++) {
            for (int j = _width * _progress; j < _width; j++) {
                mvwprintw(_window, i, j, "=");
            }
        }
        wattroff(_window, COLOR_PAIR(1));
        init_pair(2, _bgColor, _color);
        wattron(_window, COLOR_PAIR(2));
        for (int i = 0; i < _height; i++) {
            for (int j = 0; j < _width * _progress; j++) {
                mvwprintw(_window, i, j, " ");
            }
        }
        wattroff(_window, COLOR_PAIR(2));
        wrefresh(_window);
        _commit = false;
    }
}

void ProgressBar::handleEvent(int ch, MEVENT &event) {
    if (ch == KEY_MOUSE && (event.bstate & BUTTON1_PRESSED)) {
        if (event.y >= getTopLeftY() &&
            event.y < getTopLeftY() + getHeight() &&
            event.x >= getTopLeftX() &&
            event.x < getTopLeftX() + getWidth())
        {
            _progress = static_cast<double>(event.x - getTopLeftX()) / _width;
            _onTouch(_progress);
            _commit = true;
        }
    }
}

int ProgressBar::getTopLeftY() {
    if (_parent) return _parent->getTopLeftY() + _marginTop;
    return _marginTop;
}

int ProgressBar::getTopLeftX() {
    if (_parent) return _parent->getTopLeftX() + _marginLeft;
    return _marginLeft;
}

void ProgressBar::onUpdateCallback(double progress) {
    _progress = std::clamp(progress, 0.0, 1.0);
    _commit = true;
}

