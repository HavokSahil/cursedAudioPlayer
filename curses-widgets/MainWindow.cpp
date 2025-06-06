#include "MainWindow.h"
#include <ncurses.h>
#include <curses.h>

#include "Colors.h"

MainWindow::MainWindow() {
    initscr();
    cbreak();
    noecho();
    timeout(_timeoutMs);
    curs_set(0);
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0);
    getmaxyx(stdscr, _height, _width);
    start_color();

    init_pair(COLOR_BLACK_ON_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_WHITE_ON_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_GREEN_ON_BLACK, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_BLACK_ON_GREEN, COLOR_BLACK, COLOR_GREEN);

    box(stdscr, 0, 0);
}

MainWindow::~MainWindow() {
    endwin();
    refresh();
}

IWidget* MainWindow::height(int h) { _height = h; return this; }
IWidget* MainWindow::width(int w) { _width = w; return this; }
IWidget* MainWindow::parent(IWidget *p) { _parent = p; return this; }
IWidget* MainWindow::marginTop(int m) { return this; }
IWidget* MainWindow::marginLeft(int m) { return this; }
IWidget* MainWindow::timeoutMs(int ms) { _timeoutMs = ms; return this; }
IWidget* MainWindow::build() { return this; }


void MainWindow::setState(WidgetState s) { _state = s; }
void MainWindow::update() {
    if (_commit) {
        wrefresh(stdscr);
        _commit = false;
    }
    for (auto const &widget: _widgets) {
        widget->update();
    }
}

void MainWindow::handleEvent(int ch, MEVENT &event) {
    if (ch == 'q') {
        _running = false;
    }
    for (auto const &widget: _widgets) {
        widget->handleEvent(ch, event);
    }
}

void MainWindow::add(std::shared_ptr<IWidget> child) {
    child->parent(this);
    _widgets.push_back(std::move(child));
}

int MainWindow::getHeight() { return _height; }
int MainWindow::getWidth() { return _width; }
IWidget* MainWindow::getParent() { return _parent; }
int MainWindow::getMarginTop() { return 0; }
int MainWindow::getMarginLeft() { return 0; }
int MainWindow::getTopLeftY() {
    if (_parent) return _parent->getTopLeftY() + _marginTop;
    return _marginTop; }
int MainWindow::getTopLeftX() {
    if (_parent) return _parent->getTopLeftX() + _marginLeft;
    return _marginLeft; }
WidgetState MainWindow::getState() { return _state; }

void MainWindow::run() {
    while (_running) {
        int ch = getch();
        update();
        getmouse(&_event);
        handleEvent(ch, _event);
    }
}

bool MainWindow::isRunning() { return _running; }

