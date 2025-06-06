#include "Column.h"
#include <algorithm>

Column::Column() {}

Column::~Column() {
    if (_window != nullptr) delwin(_window);
}

IWidget* Column::height(int h) {
    _height = (h == -1) ? (_parent != nullptr) ? _parent->getHeight() : 0 : h;
    return this;
}

IWidget* Column::width(int w) {
    _width = (w == -1) ? (_parent != nullptr) ? _parent->getWidth() : 0 : w;
    return this;
}

IWidget* Column::parent(IWidget *p) {
    _parent = p;
    return this;
}

IWidget* Column::marginTop(int m) {
    _marginTop = m;
    return this;
}

IWidget* Column::marginLeft(int m) {
    _marginLeft = m;
    return this;
}

IWidget* Column::spacing(int s) {
    _spacing = s;
    return this;
}

IWidget* Column::mainAxisAlignment(MainAxisAlignment maa) {
    _mainAxisAlignment = maa;
    return this;
}

IWidget* Column::crossAxisAlignment(CrossAxisAlignment caa) {
    _crossAxisAlignment = caa;
    return this;
}

IWidget* Column::build() {
    if (_window == nullptr)
        _window = newwin(_height, _width, getTopLeftY(), getTopLeftX());
    else {
        mvwin(_window, getTopLeftY(), getTopLeftX());
    }
    return this;
}

void Column::setState(WidgetState s) {
    _state = s;
}

void Column::update() {
    for (auto const &child: _children) {
        child->update();
    }
}

void Column::handleEvent(int ch, MEVENT &event) {
    for (auto const &child: _children) {
        child->handleEvent(ch, event);
    }
}

void Column::add(std::shared_ptr<IWidget> child) {
    int offsetY = 0;
    int offsetX = 0;
    int childrenHeight = child->getHeight();
    for (auto const &x: _children) {
        offsetY += _spacing + x->getHeight();
        childrenHeight += x->getHeight() + _spacing;
    }

    printf("Height: %d, ChildrenHeigt: %d", _height, childrenHeight);
    // printf("Width: %d, ChildrenWidth: %d", _width, childrenHeight);

    if (_mainAxisAlignment == MX_CENTER)
        offsetY += std::max(0, (_height - childrenHeight) / 2);
    else if (_mainAxisAlignment == MX_END)
        offsetY += (_height - childrenHeight);

    if (_crossAxisAlignment == CRX_CENTER)
        offsetX += std::max(0, (_width - child->getWidth()) / 2);
    else if (_crossAxisAlignment == CRX_END)
        offsetX += std::max(0, _width - child->getWidth());

    printf("OffsetY: %d", offsetY);
    printf("OffsetX: %d", offsetX);

    child->parent(this)
        ->marginTop(offsetY)
        ->marginLeft(offsetX)
        ->build();

    _children.push_back(std::move(child));
}

int Column::getHeight() {
    return _height;
}

int Column::getWidth() {
    return _width;
}

IWidget* Column::getParent() {
    return _parent;
}

int Column::getMarginTop() {
    return _marginTop;
}

int Column::getMarginLeft() {
    return _marginLeft;
}

int Column::getTopLeftY() {
    if (_parent) return _parent->getTopLeftY() + _marginTop;
    return _marginTop;
}

int Column::getTopLeftX() {
    if (_parent) return _parent->getTopLeftX() + _marginLeft;
    return _marginLeft;
}

int Column::getSpacing() {
    return _spacing;
}

WidgetState Column::getState() {
    return _state;
}

