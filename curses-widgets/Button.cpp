#include "Button.h"
#include <algorithm>

Button::~Button() {
    if (_window)
        delwin(_window);
}

void Button::update() {
    bool st = _getStatusCb();
    if (st != _active) {
        _active = st;
        _commit = true;
    }
    if (!_window) return;
    if (_commit) {
        std::string text = _active ? _activeText : _inactiveText;
        int startX =
            std::max(0, (_width - static_cast<int>(text.length())) / 2);
        int startY = _height / 2;

        init_pair(_id, _active ? _bgColor : _color,
                  _active ? _color : _bgColor);
        wattron(_window, COLOR_PAIR(_id));
        wattron(_window, A_BOLD);
        box(_window, 0, 0);
        mvwprintw(_window, startY, 1, "%-*s", std::max(_width - 2, 0), " ");
        mvwprintw(_window, startY, startX, "%s",
                  text.substr(0, _width).c_str());
        wattroff(_window, A_BOLD);
        wattroff(_window, COLOR_PAIR(_id));
        wnoutrefresh(_window);
        _commit = false;
    }
}

void Button::handleEvent(int ch, MEVENT &event) {
    if (!_window) return;
    if (ch == KEY_MOUSE && (event.bstate & BUTTON1_PRESSED)) {
        if (event.y >= getTopLeftY() && event.y < getTopLeftY() + getmaxy(_window) &&
            event.x >= getTopLeftX() && event.x < getTopLeftX() + getmaxx(_window)) {
            _active = !_getStatusCb();
            _callback(_active);
            _commit = true;
        }
    } else if (_triggerKey != 0 && ch == _triggerKey) {
        _active = !_getStatusCb();
        _callback(_active);
        _commit = true;
    }
}
