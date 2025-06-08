#pragma once

#include <memory>
#include <vector>
#include <ncurses.h>
#include "Spacing.h"
#include "Widget.h"

typedef std::vector<std::shared_ptr<Widget>> Children;

class Row final: public Widget {
public:
    Row() {}
    ~Row() override;

    Widget* spacing(int s) { _spacing = s; return this; }
    Widget* mainAxisAlignment(MainAxisAlignment maa) { _mainAxisAlignment = maa; return this; }
    Widget* crossAxisAlignment(CrossAxisAlignment caa) { _crossAxisAlignment = caa; return this; }

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<Widget> child) override;

    int getSpacing();

private:
    int _spacing{0};
    MainAxisAlignment _mainAxisAlignment{MX_CENTER};
    CrossAxisAlignment _crossAxisAlignment{CRX_CENTER};
};