#pragma once

#include <memory>
#include <vector>
#include <ncurses.h>
#include "IWidget.h"
#include "Spacing.h"

typedef std::vector<std::shared_ptr<IWidget>> Children;

class Column final: public IWidget {
public:
    Column();
    ~Column() override;

    IWidget* height(int h) override;
    IWidget* width(int w) override;
    IWidget* parent(IWidget *p) override;
    IWidget* marginTop(int m) override;
    IWidget* marginLeft(int m) override;
    IWidget* spacing(int s);
    IWidget* mainAxisAlignment(MainAxisAlignment maa);
    IWidget* crossAxisAlignment(CrossAxisAlignment caa);
    IWidget* build() override;
    void setState(WidgetState s) override;
    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<IWidget> child) override;

    int getHeight() override;
    int getWidth() override;
    IWidget* getParent() override;
    int getMarginTop() override;
    int getMarginLeft() override;
    int getTopLeftY() override;
    int getTopLeftX() override;
    int getSpacing();
    WidgetState getState() override;

private:
    WINDOW* _window{nullptr};
    Children _children;

    int _height{0};
    int _width{0};
    IWidget* _parent{nullptr};
    int _marginTop{0};
    int _marginLeft{0};
    int _spacing{0};
    MainAxisAlignment _mainAxisAlignment{MX_CENTER};
    CrossAxisAlignment _crossAxisAlignment{CRX_CENTER};
    WidgetState _state{READY};
};
