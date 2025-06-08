#include "BoxedContainer.h"
#include <algorithm>

BoxedContainer::~BoxedContainer() {
    if (_window != nullptr) delwin(_window);
}

void BoxedContainer::update() {
    if (_commit) {
        wclear(_window);
        box(_window, 0, 0);
        wrefresh(_window);
        _commit = false;
    }
    for (auto const &child: _children)
        child->update();
}

void BoxedContainer::handleEvent(int ch, MEVENT &event) {
    for (auto &child: _children) {
        child->handleEvent(ch, event);
    }
}

void BoxedContainer::add(std::shared_ptr<Widget> child) {
    int offsetY = std::max(0, (_height - child->getHeight()) / 2);
    int offsetX = std::max(0, (_width - child->getWidth()) / 2);

    child->parent(this)
        ->marginTop(offsetY)
        ->marginLeft(offsetX)
        ->build();

    _children.push_back(std::move(child));
}
