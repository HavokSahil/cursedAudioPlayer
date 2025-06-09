#include "Column.h"
#include <algorithm>

Column::~Column() {
    if (_window != nullptr) delwin(_window);
}

Column* Column::spacing(int s) {
    _spacing = s;
    return this;
}

Column* Column::mainAxisAlignment(MainAxisAlignment maa) {
    _mainAxisAlignment = maa;
    return this;
}

Column* Column::crossAxisAlignment(CrossAxisAlignment caa) {
    _crossAxisAlignment = caa;
    return this;
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

void Column::add(std::shared_ptr<Widget> child) {
    child->parent(this);  // Associate parent now
    _children.push_back(std::move(child));  // Add child to the container

    const int colHeight = getHeight();
    const int colWidth = getWidth();

    // 1. Calculate total height of all children + spacing
    int totalChildrenHeight = 0;
    int totalTrueHeight = 0;
    for (size_t i = 0; i < _children.size(); ++i) {
        totalChildrenHeight += _children[i]->getHeight();
        totalTrueHeight += _children[i]->getHeight();
        if (i > 0)
            totalChildrenHeight += _spacing;
    }

    // 2. Determine starting Y offset based on main axis alignment
    int startOffsetY = 0;
    if (_mainAxisAlignment == MX_CENTER) {
        startOffsetY = std::max(0, (colHeight - totalChildrenHeight) / 2);
    } else if (_mainAxisAlignment == MX_END) {
        startOffsetY = std::max(0, colHeight - totalChildrenHeight);
    } else if (_mainAxisAlignment == MX_SPACE_BETWEEN && _children.size() > 1) {
        _spacing = (colHeight - totalTrueHeight) / (_children.size() - 1);
    }

    // 3. Re-layout all children
    int offsetY = startOffsetY;
    for (auto& ch : _children) {
        int offsetX = 0;

        // Cross axis alignment (horizontal)
        int chWidth = ch->getWidth();
        if (_crossAxisAlignment == CRX_CENTER) {
            offsetX = std::max(0, (colWidth - chWidth) / 2);
        } else if (_crossAxisAlignment == CRX_END) {
            offsetX = std::max(0, colWidth - chWidth);
        }

        // Convert to relative offsets
        double offsetXRel = colWidth > 0 ? static_cast<double>(offsetX) / colWidth : 0.0;
        double offsetYRel = colHeight > 0 ? static_cast<double>(offsetY) / colHeight : 0.0;

        ch->marginTopRel(offsetYRel)
          ->marginLeftRel(offsetXRel);

        offsetY += ch->getHeight() + _spacing;
    }
}

int Column::getSpacing() {
    return _spacing;
}
