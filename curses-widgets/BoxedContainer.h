#pragma once

#include <memory>
#include <ncurses.h>
#include "Widget.h"

typedef std::shared_ptr<Widget> Child;

class BoxedContainer final: public Widget {
public:
    BoxedContainer() = default;
    ~BoxedContainer() override;

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<Widget> child) override;
};
