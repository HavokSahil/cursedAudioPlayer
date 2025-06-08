#include "ProgressBar.h"
#include <algorithm>

ProgressBar::~ProgressBar() {
    if (_window) delwin(_window);
}

void ProgressBar::update() {
    double prog = _getProgressCallback();
    if (prog != _progress) { _progress = prog; _commit = true; }
    if (_commit) {
        wclear(_window);
        init_pair(1, _color, _bgColor);
        wattron(_window, COLOR_PAIR(1));
        for (int i = 0; i < getHeight(); i++) {
            for (int j = static_cast<int>(getWidth() * _progress); j < getWidth(); j++) {
                mvwprintw(_window, i, j, "=");
            }
        }
        wattroff(_window, COLOR_PAIR(1));
        init_pair(2, _bgColor, _color);
        wattron(_window, COLOR_PAIR(2));
        for (int i = 0; i < getHeight(); i++) {
            for (int j = 0; j < static_cast<int>(getWidth() * _progress); j++) {
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
            _progress = static_cast<double>(event.x - getTopLeftX()) / getWidth();
            _onTouch(_progress);
            _commit = true;
        }
    }
}

void ProgressBar::onUpdateCallback(double progress) {
    _progress = std::clamp(progress, 0.0, 1.0);
    _commit = true;
}

