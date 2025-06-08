#include "CursedLayout.h"

#include "BoxedContainer.h"
#include "Column.h"
#include "ProgressBar.h"
#include "Button.h"
#include "Row.h"
#include "TextBox.h"

// Static member definition
CursedLayout* CursedLayout::instance = nullptr;

void CursedLayout::mount() {
    mainWindow = std::make_unique<MainWindow>();
    mainWindow->build();

    /// @brief Main-Column widget
    auto mainCol = std::make_shared<Column>();
    mainCol->parent(mainWindow.get());
    mainWindow->add(mainCol);

    /// @brief First row
    auto metaRow = std::make_shared<Row>();
    metaRow->parent(mainCol.get())->heightRel(0.3)->widthRel(0.9);
    metaRow->spacing(2);
    mainCol->add(metaRow);

    /// @brief Statistical and Analysis Row
    auto statRow = std::make_shared<Row>();
    statRow->parent(mainCol.get())->heightRel(0.3)->widthRel(0.9);
    statRow->spacing(2);
    mainCol->add(statRow);

    /// @brief Progress Bar Column
    auto pbCol = std::make_shared<Column>();
    pbCol->parent(mainCol.get())->height(7)->widthRel(0.9);
    pbCol->spacing(1);
    mainCol->add(pbCol);

    auto pbWid = std::make_shared<ProgressBar>();
    pbWid->parent(mainCol.get())->height(2)->widthRel(0.9);
    pbWid->color(COLOR_GREEN);
    pbWid->bgColor(COLOR_BLACK);
    pbWid->onTouch(std::move(setTimeCallback));
    pbWid->getProgressCb(std::move(getTimeCallback));
    pbCol->add(pbWid);

    auto pbTextRow = std::make_shared<Row>();
    pbTextRow->parent(pbCol.get())->height(1)->widthRel(0.9);
    pbCol->add(pbTextRow);

    auto textCurTime = std::make_shared<TextBox>();
    textCurTime->parent(pbTextRow.get())->height(1)->width(20);
    textCurTime->color(COLOR_RED);
    textCurTime->bgColor(COLOR_BLACK);
    textCurTime->getTextCb(std::move(getElapsedSecString));
    pbTextRow->add(textCurTime);

    auto textTotTime = std::make_shared<TextBox>();
    textTotTime->parent(pbTextRow.get())->height(1)->width(20);
    textTotTime->color(COLOR_RED);
    textTotTime->bgColor(COLOR_BLACK);
    textTotTime->getTextCb(std::move(getTotalSecString));
    pbTextRow->add(textTotTime);

    auto btnRow = std::make_shared<Row>();
    btnRow->parent(mainCol.get())->height(5)->widthRel(0.9);
    btnRow->spacing(2);
    mainCol->add(btnRow);

    auto btnPrev10s = std::make_shared<Button>();
    btnPrev10s->parent(mainCol.get())->height(3)->width(10);
    btnPrev10s->activeText("*");
    btnPrev10s->inactiveText("<< 10s");
    btnPrev10s->onClick([this](bool _) { prevSecCallback(10); });
    btnPrev10s->getStatusCb([]()->bool { return false; });
    btnPrev10s->color(COLOR_RED);
    btnRow->add(btnPrev10s);

    auto btnPlay = std::make_shared<Button>();
    btnPlay->parent(mainCol.get())->height(3)->width(10);
    btnPlay->activeText("Pause");
    btnPlay->inactiveText("Play");
    btnPlay->onClick(std::move(setPlaybackCallback));
    btnPlay->getStatusCb(std::move(getPlaybackCallback));
    btnRow->add(btnPlay);

    auto btnNext10s = std::make_shared<Button>();
    btnNext10s->parent(mainCol.get())->height(3)->width(10);
    btnNext10s->activeText("*");
    btnNext10s->inactiveText("10s >>");
    btnNext10s->onClick([this](bool _) { nextSecCallback(10); });
    btnNext10s->getStatusCb([]()->bool { return false; });
    btnNext10s->color(COLOR_RED);
    btnRow->add(btnNext10s);

    mainWindow->resize();
}

void CursedLayout::resize() {
    mainWindow->resize();
}

void CursedLayout::run() {
    mainWindow->run();
}
