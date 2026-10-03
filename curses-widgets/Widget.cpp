#include "Widget.h"

#include <algorithm>

Widget* Widget::height(int h) {
    _height = h;
    return this;
}

Widget* Widget::width(int w) {
    _width = w;
    return this;
}

Widget *Widget::heightRel(double h) {
    _heightRel = std::clamp(h, 0.0, 1.0);
    return this;
}

Widget* Widget::widthRel(double w) {
    _widthRel = std::clamp(w, 0.0, 1.0);
    return this;
}

Widget* Widget::parent(Widget *p) {
    _parent = p;
    return this;
}

Widget* Widget::marginTop(int m) {
    _marginTop = m;
    return this;
}

Widget* Widget::marginTopRel(double m) {
    _marginTopRel = m;
    return this;
}

Widget* Widget::marginLeft(int m) {
    _marginLeft = m;
    return this;
}

Widget *Widget::marginLeftRel(double m) {
    _marginLeftRel = m;
    return this;
}

WINDOW* Widget::_createWindow() {
    const int y = getTopLeftY();
    const int x = getTopLeftX();
    int height = std::min(getHeight(), getmaxy(stdscr) - y);
    int width = std::min(getWidth(), getmaxx(stdscr) - x);
    if (_parent) {
        if (!_parent->_window) return nullptr;
        height = std::min(height, _parent->getTopLeftY() + getmaxy(_parent->_window) - y);
        width = std::min(width, _parent->getTopLeftX() + getmaxx(_parent->_window) - x);
    }
    if (height <= 0 || width <= 0) return nullptr;
    WINDOW* window = newwin(height, width, y, x);
    if (window) leaveok(window, TRUE);
    return window;
}

Widget* Widget::build() {
    if (!_window) _window = _createWindow();
    else mvwin(_window, getTopLeftY(), getTopLeftX());
    return this;
}

void Widget::setState(WidgetState s) {
    _state = s;
}

void Widget::clear() {
    if (_window) werase(_window);
}

void Widget::resize() {
    if (_window) delwin(_window);
    _window = _createWindow();
    _commit = true;
    for (auto const &child: _children) {
        child->resize();
    }
}

int Widget::getHeight() {
    if (_height != -1) return _height;
    if (_heightRel == 0.0) return _parent ? _parent->getHeight() : getmaxy(stdscr);
    if (_parent) return static_cast<int>(_parent->getHeight() * _heightRel);
    return static_cast<int>(getmaxy(stdscr) * _heightRel);
}

int Widget::getWidth() {
    if (_width != -1) return _width;
    if (_widthRel == 0.0) return _parent ? _parent->getWidth() : getmaxx(stdscr);
    if (_parent) return static_cast<int>(_parent->getWidth() * _widthRel);
    return static_cast<int>(getmaxx(stdscr) * _widthRel);
}

Widget* Widget::getParent() {
    return _parent;
}

int Widget::getMarginTop() {
    if (_marginTop != -1) return _marginTop;
    if (_parent) return static_cast<int>(_parent->getHeight() * _marginTopRel);
    return static_cast<int>(getmaxy(stdscr) * _marginTopRel);
}

int Widget::getMarginLeft() {
    if (_marginLeft != -1) return _marginLeft;
    if (_parent) return static_cast<int>(_parent->getWidth() * _marginLeftRel);
    return static_cast<int>(getmaxx(stdscr) * _marginLeftRel);
}

int Widget::getTopLeftY() {
    if (_parent) return std::clamp(_parent->getTopLeftY() + getMarginTop(), 0, getmaxy(stdscr) - 1);
    return getMarginTop();
}

int Widget::getTopLeftX() {
    if (_parent) return std::clamp(_parent->getTopLeftX() + getMarginLeft(), 0, getmaxx(stdscr) - 1);
    return getMarginLeft();
}

WidgetState Widget::getState() {
    return _state;
}




