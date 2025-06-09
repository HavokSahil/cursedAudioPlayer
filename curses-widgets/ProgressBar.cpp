#include "ProgressBar.h"
#include <algorithm>

ProgressBar::~ProgressBar() {
    if (_window) delwin(_window);
}

void ProgressBar::update() {

    if (color_init) {
        init_pair(_id, _color, _bgColor);
        init_pair(_id + 1, _bgColor, _color);
        color_init = false;
    }


    double prog = _getProgressCallback();
    if (prog != _progress) { _progress = prog; _commit = true; }
    if (_commit) {
        wclear(_window);
        wattron(_window, COLOR_PAIR(_id));
        for (int i = 0; i < getHeight(); i++) {
            for (int j = static_cast<int>(getWidth() * _progress); j < getWidth(); j++) {
                mvwprintw(_window, i, j, "=");
            }
        }
        wattroff(_window, COLOR_PAIR(_id));
        wattron(_window, COLOR_PAIR(_id + 1));
        for (int i = 0; i < getHeight(); i++) {
            for (int j = 0; j < static_cast<int>(getWidth() * _progress); j++) {
                mvwprintw(_window, i, j, " ");
            }
        }
        wattroff(_window, COLOR_PAIR(_id + 1));
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

