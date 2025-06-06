#include "Button.h"
#include <algorithm>

Button::~Button() {
    if (_window) delwin(_window);
}

IWidget* Button::build() {
    if (_window == nullptr)
        _window = newwin(_height, _width, getTopLeftY(), getTopLeftX());
    else {
        mvwin(_window, getTopLeftY(), getTopLeftX());
    }
    return this;
}

void Button::update() {
    bool st = _getStatusCb();
    if (st != _active) { _active = st; _commit = true; }
    if (_commit) {
        std::string text = _active? _activeText: _inactiveText;
        int startX = std::max(0, (_width - static_cast<int>(text.length())) / 2);
        int startY = _height / 2;

        init_pair(3, _active? _bgColor: _color, _active? _color: _bgColor);
        wattron(_window, COLOR_PAIR(3));
        wattron(_window, A_BOLD);
        box(_window, 0, 0);
        mvwprintw(_window, startY, 1, "%-*s", std::max(_width-2, 0), " ");
        mvwprintw(_window, startY, startX, "%s", text.substr(0, _width).c_str());
        wattroff(_window, A_BOLD);
        wattroff(_window, COLOR_PAIR(3));
        wrefresh(_window);
        _commit = false;
    }
}

void Button::handleEvent(int ch, MEVENT &event) {
    if (ch == KEY_MOUSE && (event.bstate & BUTTON1_PRESSED)) {
        if (event.y >= getTopLeftY() &&
            event.y < getTopLeftY() + getHeight() &&
            event.x >= getTopLeftX() &&
            event.x < getTopLeftX() + getWidth())
        {
            _active = !_active;
            _callback(_active);
            _commit = true;
        }
    }
}

int Button::getTopLeftY() {
    if (_parent) return _parent->getTopLeftY() + _marginTop;
    return _marginTop;
}

int Button::getTopLeftX() {
    if (_parent) return _parent->getTopLeftX() + _marginLeft;
    return _marginLeft;
}

