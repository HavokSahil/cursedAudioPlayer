#include "BoxedContainer.h"
#include "Colors.h"
#include <algorithm>

BoxedContainer::~BoxedContainer() {
    if (_window) delwin(_window);
}

void BoxedContainer::update() {
    if (_window && _commit) {
        werase(_window);
        wattron(_window, COLOR_PAIR(COLOR_GREEN_ON_BLACK));
        box(_window, 0, 0);
        if (getmaxx(_window) > 4)
            mvwaddnstr(_window, 0, 2, _title.c_str(), getmaxx(_window) - 4);
        wattroff(_window, COLOR_PAIR(COLOR_GREEN_ON_BLACK));
        wnoutrefresh(_window);
        _commit = false;
    }
    for (auto const &child: _children) child->update();
}

void BoxedContainer::handleEvent(int ch, MEVENT &event) {
    for (auto const &child: _children) child->handleEvent(ch, event);
}

void BoxedContainer::add(std::shared_ptr<Widget> child) {
    child->parent(this);
    _children.push_back(std::move(child));
}

void BoxedContainer::resize() {
    for (auto const &child: _children) {
        child->width(std::max(1, getWidth() - 2))->height(std::max(1, getHeight() - 2));
        child->marginTop(1)->marginLeft(1);
    }
    Widget::resize();
}
