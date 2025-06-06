#pragma once

#include <memory>
#include <vector>
#include <ncurses.h>
#include "IWidget.h"
#include "Spacing.h"

typedef std::vector<std::shared_ptr<IWidget>> Children;

class Row final: public IWidget {
public:
    Row() = default;
    ~Row() override;

    IWidget* height(int h) override { _height = (h == -1)? (_parent != nullptr)? _parent->getHeight(): 0: h; return this; }
    IWidget* width(int w) override { _width = (w == -1)? (_parent != nullptr)? _parent->getWidth(): 0: w; return this; }
    IWidget* parent(IWidget *p) override { _parent = p; return this; }
    IWidget* marginTop(int m) override;
    IWidget* marginLeft(int m) override { _marginLeft = m; return this; }
    IWidget* spacing(int s) { _spacing = s; return this; }
    IWidget* mainAxisAlignment(MainAxisAlignment maa) { _mainAxisAlignment = maa; return this; }
    IWidget* crossAxisAlignment(CrossAxisAlignment caa) { _crossAxisAlignment = caa; return this; }
    IWidget* build() override;
    void setState(WidgetState s) override { _state = s; }
    void update() override;
    void handleEvent(int ch, MEVENT &event) override;
    void add(std::shared_ptr<IWidget> child) override;

    int getHeight() override { return _height; }
    int getWidth() override { return _width; }
    IWidget* getParent() override { return _parent; }
    int getMarginTop() override { return _marginTop;}
    int getMarginLeft() override { return _marginLeft; }
    int getTopLeftY() override;
    int getTopLeftX() override;
    int getSpacing() { return _spacing; }
    WidgetState getState() override { return _state; }

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
    bool _commit{true};
};