#include "BoxedContainer.h"
#include <algorithm>

BoxedContainer::BoxedContainer() {}

BoxedContainer::~BoxedContainer() {
    if (_window != nullptr) delwin(_window);
}

IWidget* BoxedContainer::height(int h) {
    _height = (h == -1) ? (_parent != nullptr) ? _parent->getHeight() : 0 : h;
    return this;
}

IWidget* BoxedContainer::width(int w) {
    _width = (w == -1) ? (_parent != nullptr) ? _parent->getWidth() : 0 : w;
    return this;
}

IWidget* BoxedContainer::parent(IWidget *p) {
    _parent = p;
    return this;
}

IWidget* BoxedContainer::marginTop(int m) {
    _marginTop = m;
    return this;
}

IWidget* BoxedContainer::marginLeft(int m) {
    _marginLeft = m;
    return this;
}

IWidget* BoxedContainer::build() {
    if (_window == nullptr) {
        _window = newwin(_height, _width, getTopLeftY(), getTopLeftX());
        box(_window, 0, 0);
    }
    else {
        mvwin(_window, getTopLeftY(), getTopLeftX());
    }
    return this;
}

void BoxedContainer::setState(WidgetState s) {
    _state = s;
}

void BoxedContainer::update() {
    box(_window, 0, 0);
    _child->update();
}

void BoxedContainer::handleEvent(int ch, MEVENT &event) {
    _child->handleEvent(ch, event);
}

void BoxedContainer::add(std::shared_ptr<IWidget> child) {
    int offsetY = std::max(0, (_height - child->getHeight()) / 2);
    int offsetX = std::max(0, (_width - child->getWidth()) / 2);

    child->parent(this)
        ->marginTop(offsetY)
        ->marginLeft(offsetX)
        ->build();

    _child = std::move(child);
}

int BoxedContainer::getHeight() {
    return _height;
}

int BoxedContainer::getWidth() {
    return _width;
}

IWidget* BoxedContainer::getParent() {
    return _parent;
}

int BoxedContainer::getMarginTop() {
    return _marginTop;
}

int BoxedContainer::getMarginLeft() {
    return _marginLeft;
}

int BoxedContainer::getTopLeftY() {
    if (_parent) return _parent->getTopLeftY() + _marginTop;
    return _marginTop;
}

int BoxedContainer::getTopLeftX() {
    if (_parent) return _parent->getTopLeftX() + _marginLeft;
    return _marginLeft;
}

WidgetState BoxedContainer::getState() {
    return _state;
}

