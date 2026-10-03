//
// Created by havoksahil on 6/8/25.
//

#include "TextBox.h"
#include <algorithm>

TextBox::~TextBox() {
    if (_window) delwin(_window);
}

void TextBox::update() {
    if (color_init) {
        init_pair(_id, _color, _bgColor);
        init_pair(_id + 1, _bgColor, _color);
        color_init = false;
    }
    std::string s = _getTextCb();
    if (s != _text) {
        _text = s;
        _commit = true;
    }
    if (!_window) return;
    _strOffset = std::clamp(_strOffset, 0, static_cast<int>(s.size()));
    int mlen = std::max(0, std::min(getmaxx(_window) - 2 * _padding, static_cast<int>(s.length()) - _strOffset));
    if (_commit) {
        werase(_window);
        wattron(_window, COLOR_PAIR(_selected? _id + 1: _id));
        wattron(_window, A_BOLD);
        mvwprintw(_window, 0, _padding, "%s", s.substr(_strOffset, mlen).c_str());
        wattroff(_window, A_BOLD);
        wattroff(_window, COLOR_PAIR(_selected? _id + 1: _id));
        wnoutrefresh(_window);
        _commit = false;
    }
}

void TextBox::handleEvent(int ch, MEVENT &event) {
    if (!_window) return;
    if (_selected) {
        if (ch == '[') {
            _strOffset = std::max(_strOffset - 1, 0); _commit = true;
        } else if (ch == ']' && static_cast<int>(_text.length()) - _strOffset > std::max(0, getWidth() - 2 * _padding)) {
            _strOffset++; _commit = true;
        }
    }
    if (ch == KEY_MOUSE && (event.bstate & BUTTON1_PRESSED)) {
        if (event.y >= getTopLeftY() &&
            event.y < getTopLeftY() + getmaxy(_window) &&
            event.x >= getTopLeftX() &&
            event.x < getTopLeftX() + getmaxx(_window))
        {
            _selected = !_selected;
        } else {
            _selected = false;
        }
        _commit = true;
    }
}

