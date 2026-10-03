#pragma once
#include <string>

#include <memory>
#include <ncurses.h>
#include "Widget.h"

typedef std::shared_ptr<Widget> Child;

class BoxedContainer final: public Widget {
public:
    BoxedContainer() = default;
    ~BoxedContainer() override;

    BoxedContainer* title(const char* value) { _title = value; return this; }
    void resize() override;
    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<Widget> child) override;
private:
    std::string _title;
};
