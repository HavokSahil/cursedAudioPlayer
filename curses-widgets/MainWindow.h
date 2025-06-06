#pragma once

#include <memory>
#include <vector>

#include "IWidget.h"

class MainWindow final: public IWidget {
public:
    MainWindow();
    ~MainWindow() override;

    IWidget* height(int h) override;
    IWidget* width(int w) override;
    IWidget* parent(IWidget *p) override;
    IWidget* marginTop(int m) override;
    IWidget* marginLeft(int m) override;
    IWidget* timeoutMs(int ms);
    IWidget* build() override;

    void setState(WidgetState) override;
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

    void run();
    bool isRunning();
private:
    MEVENT _event{};

    std::vector<std::shared_ptr<IWidget>> _widgets;
    bool _running{true};
    int _height{0};
    int _width{0};
    IWidget* _parent{nullptr};
    int _marginTop{0};
    int _marginLeft{0};
    uint64_t _timeoutMs{50};
    WidgetState _state{READY};
    bool _commit{true};
};
