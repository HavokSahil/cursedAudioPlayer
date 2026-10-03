#include "MainWindow.h"
#include <ncurses.h>
#include <algorithm>
#include <chrono>

#include "Colors.h"

MainWindow::MainWindow() {
    initscr();
    cbreak();
    noecho();
    timeout(_timeoutMs);
    curs_set(0);
    leaveok(stdscr, TRUE);
#ifdef NCURSES_VERSION
    set_escdelay(25);
#endif
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
    _keyCallback = {};
    _children.clear();
    if (_window) delwin(_window);
    endwin();
}

Widget *MainWindow::timeoutMs(int ms) {
    _timeoutMs = std::max(1, ms);
    timeout(_timeoutMs);
    return this;
}

void MainWindow::update() {
    for (auto const &child: _children) {
        child->update();
    }
}


void MainWindow::handleEvent(int ch, MEVENT &event) {
    if (ch == KEY_RESIZE) {
        erase();
        wnoutrefresh(stdscr);
        resize();
        return;
    }
    if (ch == 'q' || ch == 27) {
        _running = false;
    }
    _keyCallback(ch);
    for (auto const &child: _children) {
        child->handleEvent(ch, event);
    }
}

void MainWindow::add(std::shared_ptr<Widget> child) {
    child->parent(this);
    _children.push_back(std::move(child));
}

void MainWindow::run() {
    using Clock = std::chrono::steady_clock;
    auto nextFrame = Clock::now();
    _running = true;
    while (_running) {
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(nextFrame - Clock::now());
        timeout(Clock::now() < nextFrame ? std::max(1, static_cast<int>(remaining.count())) : 0);
        int ch = getch();
        _event = {};
        if (ch == KEY_MOUSE && getmouse(&_event) != OK) ch = ERR;
        if (ch != ERR) this->handleEvent(ch, _event);
        if (!_running) break;
        if (Clock::now() < nextFrame && ch != KEY_RESIZE) continue;

        if (getmaxx(stdscr) < 80 || getmaxy(stdscr) < 24) {
            erase();
            mvaddnstr(0, 0, "CURSEDAP // resize to 80 x 24 // q quit", getmaxx(stdscr) - 1);
            wnoutrefresh(stdscr);
        } else {
            this->update();
        }
        doupdate();
        nextFrame += std::chrono::milliseconds(_timeoutMs);
        if (nextFrame <= Clock::now()) nextFrame = Clock::now() + std::chrono::milliseconds(_timeoutMs);
    }
}

bool MainWindow::isRunning() { return _running; }

