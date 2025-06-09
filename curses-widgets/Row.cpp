#include "Row.h"
#include <algorithm>

Row::~Row() {
    if (_window) delwin(_window);
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

void Row::add(std::shared_ptr<Widget> child) {
    child->parent(this);  // Set parent
    _children.push_back(std::move(child));  // Add child to the container

    const int rowWidth = getWidth();
    const int rowHeight = getHeight();

    // 1. Calculate total width of all children + spacing
    int totalChildrenWidth = 0;
    int trueChildrenWidth = 0;
    for (size_t i = 0; i < _children.size(); ++i) {
        totalChildrenWidth += _children[i]->getWidth();
        trueChildrenWidth += _children[i]->getWidth();
        if (i > 0)
            totalChildrenWidth += _spacing;
    }

    // 2. Determine starting X offset based on main axis alignment
    int startOffsetX = 0;
    if (_mainAxisAlignment == MX_CENTER) {
        startOffsetX = std::max(0, (rowWidth - totalChildrenWidth) / 2);
    } else if (_mainAxisAlignment == MX_END) {
        startOffsetX = std::max(0, rowWidth - totalChildrenWidth);
    } else if (_mainAxisAlignment == MX_SPACE_BETWEEN && _children.size() > 1) {
        _spacing = (rowWidth - trueChildrenWidth) / (_children.size() - 1);
    }

    // 3. Re-layout all children (left to right)
    int offsetX = startOffsetX;
    for (auto& ch : _children) {
        int offsetY = 0;

        // Cross axis alignment (vertical)
        int chHeight = ch->getHeight();
        if (_crossAxisAlignment == CRX_CENTER) {
            offsetY = std::max(0, (rowHeight - chHeight) / 2);
        } else if (_crossAxisAlignment == CRX_END) {
            offsetY = std::max(0, rowHeight - chHeight);
        }

        // Convert to relative offsets
        double offsetXRel = rowWidth > 0 ? static_cast<double>(offsetX) / rowWidth : 0.0;
        double offsetYRel = rowHeight > 0 ? static_cast<double>(offsetY) / rowHeight : 0.0;

        ch->marginLeftRel(offsetXRel)
          ->marginTopRel(offsetYRel);

        offsetX += ch->getWidth() + _spacing;
    }
}

int Row::getSpacing() {
    return _spacing;
}
