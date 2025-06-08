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

Widget* Widget::build() {
    if (_window == nullptr)
        _window = newwin(getHeight(), getWidth(), getTopLeftY(), getTopLeftX());
    else mvwin(_window, getTopLeftY(), getTopLeftX());
    return this;
}

void Widget::setState(WidgetState s) {
    _state = s;
}

void Widget::clear() {
    wclear(_window);
}

void Widget::resize() {
    delwin(_window);
    _window = newwin(getHeight(), getWidth(), getTopLeftY(), getTopLeftX());
    _commit = true;
    for (auto const &child: _children) {
        child->resize();
    }
}

int Widget::getHeight() {
    if (_height != -1) return _height;
    if (_heightRel == 0.0) return getmaxy(stdscr);
    if (_parent) return static_cast<int>(_parent->getHeight() * _heightRel);
    return static_cast<int>(getmaxy(stdscr) * _heightRel);
}

int Widget::getWidth() {
    if (_width != -1) return _width;
    if (_widthRel == 0.0) return getmaxx(stdscr);
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
    if (_parent) return _parent->getTopLeftY() + getMarginTop();
    return getMarginTop();
}

int Widget::getTopLeftX() {
    if (_parent) return _parent->getTopLeftX() + getMarginLeft();
    return getMarginLeft();
}

WidgetState Widget::getState() {
    return _state;
}




