//
// Created by havoksahil on 6/8/25.
//

#include "TextBox.h"

TextBox::~TextBox() {
    delwin(_window);
}

void TextBox::update() {
    std::string s = _getTextCb();
    int mlen = std::min(getWidth() - 2, static_cast<int>(s.length()));
    wclear(_window);
    init_pair(_id, _color, _bgColor);
    wattron(_window, COLOR_PAIR(_id));
    wattron(_window, A_BOLD);
    mvwprintw(_window, 0, 1, "%s", s.substr(0, mlen).c_str());
    wattroff(_window, A_BOLD);
    wattroff(_window, COLOR_PAIR(_id));
    wrefresh(_window);
}

void TextBox::handleEvent(int ch, MEVENT &event) {}

