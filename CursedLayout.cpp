#include "CursedLayout.h"

#include "Column.h"
#include "ProgressBar.h"
#include "Button.h"
#include "IWidget.h"

// Static member definition
CursedLayout* CursedLayout::instance = nullptr;

void CursedLayout::mount() {
    mainWindow->build();
    auto mainCol = std::make_shared<Column>();
    mainCol
        ->parent(mainWindow.get())
        ->height(-1)
        ->width(-1)
        ->build();

    mainCol->spacing(2);
    mainCol->crossAxisAlignment(CRX_CENTER);
    mainCol->mainAxisAlignment(MX_CENTER);

    auto pb = std::make_shared<ProgressBar>();
    pb
    ->height(2)
    ->width(100)
    ->parent(mainCol.get())
    ->build();

    pb->getProgressCb( [&]()->double { return getTimeCallback(); } );
    pb->onTouch( [&](double time)-> void { setTimeCallback(time); } );
    pb->color(COLOR_RED);
    pb->bgColor(COLOR_BLACK);

    auto btn = std::make_shared<Button>();
    btn->width(14)
    ->height(3)
    ->parent(mainCol.get())
    ->build();

    btn->activeText("Pause");
    btn->inactiveText("Play");
    btn->onClick( [&](bool value) { setPlaybackCallback(value); } );
    btn->getStatusCb( [&]()->bool { return getPlaybackCallback(); } );
    btn->color(COLOR_GREEN);
    btn->bgColor(COLOR_BLACK);

    mainCol->add(pb);
    mainCol->add(btn);
    mainWindow->add(mainCol);
}

void CursedLayout::resize() {

}

void CursedLayout::run() {
    mainWindow->run();
}
