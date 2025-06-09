#include "MainWindow.h"
#include <ncurses.h>

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
    start_color();

    init_pair(COLOR_BLACK_ON_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_WHITE_ON_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_GREEN_ON_BLACK, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_BLACK_ON_GREEN, COLOR_BLACK, COLOR_GREEN);
}

MainWindow::~MainWindow() {
    endwin();
}

Widget *MainWindow::timeoutMs(int ms) {
    _timeoutMs = ms;
    return this;
}

void MainWindow::update() {
    for (auto const &child: _children) {
        child->update();
    }
}


void MainWindow::handleEvent(int ch, MEVENT &event) {
    if (ch == 'q') {
        _running = false;
    }
    for (auto const &child: _children) {
        child->handleEvent(ch, event);
    }
}

void MainWindow::add(std::shared_ptr<Widget> child) {
    child->parent(this);
    _children.push_back(std::move(child));
}

void MainWindow::run() {
    _running = true;
    while (_running) {
        int ch = getch();
        this->update();
        getmouse(&_event);
        this->handleEvent(ch, _event);
    }
}

bool MainWindow::isRunning() { return _running; }

