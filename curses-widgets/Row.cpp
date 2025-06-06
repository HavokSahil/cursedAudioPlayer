#include "Row.h"
#include <algorithm>

Row::~Row() {
    if (_window) delwin(_window);
}

IWidget* Row::marginTop(int m) {
    _marginTop = m;
    return this;
}

IWidget* Row::build() {
    if (_window == nullptr)
        _window = newwin(_height, _width, getTopLeftY(), getTopLeftX());
    else {
        mvwin(_window, getTopLeftY(), getTopLeftX());
    }
    return this;
}

void Row::update() {
    for (auto const &child: _children) {
        child->update();
    }
}

void Row::handleEvent(int ch, MEVENT &event) {
    for (auto const &child: _children) {
        child->handleEvent(ch, event);
    }
}

void Row::add(std::shared_ptr<IWidget> child) {
    int offsetY = 0;
    int offsetX = 0;
    int childrenWidth = child->getWidth();
    for (auto const &x: _children) {
        offsetX += _spacing + x->getWidth();
        childrenWidth += x->getWidth() + _spacing;
    }

    offsetX += (_mainAxisAlignment == MX_CENTER)?
        std::max(0, (_height - childrenWidth) / 2):
        (_mainAxisAlignment == MX_START)? 0:
        (_height - childrenWidth);

    offsetY += (_crossAxisAlignment == CRX_CENTER) ?
        std::max(0, (_width - child->getHeight()) / 2):
        (_crossAxisAlignment == CRX_START)? 0:
        std::max(0, _width - child->getHeight());

    child->parent(this)
        ->marginTop(offsetY)
        ->marginLeft(offsetX)
        ->build();

    _children.push_back(std::move(child));
}

int Row::getTopLeftY() {
    if (_parent) return _parent->getTopLeftY() + _marginTop;
    return _marginTop;
}

int Row::getTopLeftX() {
    if (_parent) return _parent->getTopLeftX() + _marginLeft;
    return _marginLeft;
}

