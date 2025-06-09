#pragma once

#include <memory>
#include <vector>
#include <ncurses.h>
#include "Spacing.h"
#include "Widget.h"

typedef std::vector<std::shared_ptr<Widget>> Children;

class Column final: public Widget {
public:
    Column() = default;
    ~Column() override;

    Column* spacing(int s);
    Column* mainAxisAlignment(MainAxisAlignment maa);
    Column* crossAxisAlignment(CrossAxisAlignment caa);

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<Widget> child) override;

    int getSpacing();

private:
    int _spacing{0};
    MainAxisAlignment _mainAxisAlignment{MX_CENTER};
    CrossAxisAlignment _crossAxisAlignment{CRX_CENTER};
};
