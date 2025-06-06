#pragma once

#include <curses.h>
#include <memory>

enum WidgetState {
    READY = 0,
    MOUNTED = 1,
};

class IWidget {
public:
    virtual ~IWidget() = default;

    virtual IWidget* height(int h) = 0;
    virtual IWidget* width(int w) = 0;
    virtual IWidget* parent(IWidget* p) = 0;
    virtual IWidget* marginTop(int m) = 0;
    virtual IWidget* marginLeft(int m) = 0;
    virtual IWidget* build() = 0;

    virtual void setState(WidgetState) = 0;
    virtual void update() = 0;
    virtual void handleEvent(int ch, MEVENT &event) = 0;
    virtual void add(std::shared_ptr<IWidget> child) = 0;

    virtual int getHeight() = 0;
    virtual int getWidth() = 0;
    virtual IWidget* getParent() = 0;
    virtual int getMarginTop() = 0;
    virtual int getMarginLeft() = 0;
    virtual int getTopLeftY() = 0;
    virtual int getTopLeftX() = 0;
    virtual WidgetState getState() = 0;
};