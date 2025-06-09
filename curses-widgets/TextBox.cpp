//
// Created by havoksahil on 6/8/25.
//

#include "TextBox.h"

TextBox::~TextBox() {
    delwin(_window);
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
    int mlen = std::min(getWidth() - 2, static_cast<int>(s.length()));
    if (_commit) {
        wclear(_window);
        wattron(_window, COLOR_PAIR(_selected? _id + 1: _id));
        wattron(_window, A_BOLD);
        mvwprintw(_window, 0, 1, "%s", s.substr(_strOffset, mlen).c_str());
        wattroff(_window, A_BOLD);
        wattroff(_window, COLOR_PAIR(_selected? _id + 1: _id));
        wrefresh(_window);
        _commit = false;
    }
}

void TextBox::handleEvent(int ch, MEVENT &event) {
    if (_selected) {
        if (ch == KEY_LEFT) {
            _strOffset = std::max(_strOffset - 1, 0); _commit = true;
        } else if (ch == KEY_RIGHT && (_text.length() - _strOffset >= getWidth() - 2)) {
            _strOffset++; _commit = true;
        }
    }
    if (ch == KEY_MOUSE && (event.bstate & BUTTON1_PRESSED)) {
        if (event.y >= getTopLeftY() &&
            event.y < getTopLeftY() + getHeight() &&
            event.x >= getTopLeftX() &&
            event.x < getTopLeftX() + getWidth())
        {
            _selected = !_selected;
        } else {
            _selected = false;
        }
        _commit = true;
    }
}

