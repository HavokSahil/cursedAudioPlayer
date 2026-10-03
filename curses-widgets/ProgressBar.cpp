#include "ProgressBar.h"
#include <algorithm>
#include <cmath>

ProgressBar::~ProgressBar() {
    if (_window) delwin(_window);
}

void ProgressBar::update() {
    if (!_window) return;
    if (color_init) {
        init_pair(_id, _color, _bgColor);
        color_init = false;
    }
    const int width = getmaxx(_window);
    double progress = _getProgressCallback();
    progress = std::isfinite(progress) ? std::clamp(progress, 0.0, 1.0) : 0.0;
    const int filled = static_cast<int>(progress * width);
    if (filled != static_cast<int>(_progress * width)) _commit = true;
    _progress = progress;
    if (!_commit) return;

    werase(_window);
    wattron(_window, COLOR_PAIR(_id));
    for (int x = 0; x < width; ++x) {
        if (x < filled) wattron(_window, A_BOLD);
        else wattron(_window, A_DIM);
        mvwaddch(_window, 0, x, x < filled ? ACS_CKBOARD : ACS_HLINE);
        wattroff(_window, A_BOLD | A_DIM);
    }
    if (filled < width) mvwaddch(_window, 0, filled, ACS_DIAMOND | A_BOLD);
    wattroff(_window, COLOR_PAIR(_id));
    wnoutrefresh(_window);
    _commit = false;
}

void ProgressBar::handleEvent(int ch, MEVENT &event) {
    if (ch == KEY_MOUSE && (event.bstate & BUTTON1_PRESSED) && _window) {
        const int width = getmaxx(_window);
        if (event.y >= getTopLeftY() && event.y < getTopLeftY() + getmaxy(_window) &&
            event.x >= getTopLeftX() && event.x < getTopLeftX() + width) {
            _progress = width > 1 ? static_cast<double>(event.x - getTopLeftX()) / (width - 1) : 0.0;
            _onTouch(_progress);
            _commit = true;
        }
    }
}

void ProgressBar::onUpdateCallback(double progress) {
    _progress = std::isfinite(progress) ? std::clamp(progress, 0.0, 1.0) : 0.0;
    _commit = true;
}
