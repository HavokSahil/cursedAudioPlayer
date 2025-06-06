#pragma once

#include <memory>
#include <ncurses.h>
#include "IWidget.h"

typedef std::shared_ptr<IWidget> Child;

class BoxedContainer final: public IWidget {
public:
    BoxedContainer();
    ~BoxedContainer() override;

    IWidget* height(int h) override;
    IWidget* width(int w) override;
    IWidget* parent(IWidget *p) override;
    IWidget* marginTop(int m) override;
    IWidget* marginLeft(int m) override;
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
    WidgetState getState() override;

private:
    WINDOW* _window{nullptr};
    Child _child;

    int _height{0};
    int _width{0};
    IWidget* _parent{nullptr};
    int _marginTop{0};
    int _marginLeft{0};
    WidgetState _state{READY};
};
