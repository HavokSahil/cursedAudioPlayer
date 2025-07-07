#include "CursedLayout.h"

#include "BarPlot.h"
#include "BoxedContainer.h"
#include "Button.h"
#include "Column.h"
#include "FHexStream.h"
#include "ProgressBar.h"
#include "Row.h"
#include "TextBox.h"

// Static member definition
CursedLayout *CursedLayout::instance = nullptr;

void CursedLayout::mount() {
    mainWindow = std::make_unique<MainWindow>();
    mainWindow->build();

    /// @brief Main-Column widget
    auto mainCol = std::make_shared<Column>();
    mainCol->parent(mainWindow.get())->widthRel(0.95);
    mainWindow->add(mainCol);

    /// @brief First row
    auto metaRow = std::make_shared<Row>();
    metaRow->parent(mainCol.get())->heightRel(0.3);
    metaRow->mainAxisAlignment(MX_SPACE_BETWEEN);
    mainCol->add(metaRow);

    // @brief Add file picker widget
    // TODO: Complete the hex dump widget
    auto hexStream = std::make_shared<FHexStream>();
    hexStream->parent(metaRow.get())->widthRel(0.5)->heightRel(1.0);
    hexStream->nBytes(8);
    hexStream->nLines(6);
    hexStream->bytesCb(std::move(hexDataCallback));
    hexStream->mount();
    metaRow->add(hexStream);

    // TODO: Complete the file metadata implementation
    auto metaCol = std::make_shared<Column>();
    metaCol->parent(metaRow.get())->widthRel(0.3)->heightRel(1.0);
    metaCol->mainAxisAlignment(MX_CENTER);
    metaRow->add(metaCol);

    auto titleTextBox = std::make_shared<TextBox>();
    titleTextBox->parent(metaRow.get())->height(1)->widthRel(1.0);
    titleTextBox->text("File Metadata");
    titleTextBox->color(COLOR_GREEN);
    titleTextBox->bgColor(COLOR_BLACK);
    metaCol->add(titleTextBox);

    auto addMetaDataRow = [&](const char *label, const char *dValue,
                              std::function<std::string()> &&callback) {
        /// @brief Sample Rate
        auto outerRow = std::make_shared<Row>();
        outerRow->parent(metaCol.get())->widthRel(1.0)->height(1);
        outerRow->mainAxisAlignment(MX_SPACE_BETWEEN);
        metaCol->add(outerRow);

        auto labelWidget = std::make_shared<TextBox>();
        labelWidget->parent(outerRow.get())->height(1)->widthRel(0.3);
        labelWidget->text(label);
        labelWidget->color(COLOR_GREEN);
        labelWidget->bgColor(COLOR_BLACK);
        outerRow->add(labelWidget);

        auto valueWidget = std::make_shared<TextBox>();
        valueWidget->parent(outerRow.get())->height(1)->widthRel(0.7);
        valueWidget->text(dValue);
        valueWidget->color(COLOR_RED);
        valueWidget->bgColor(COLOR_BLACK);
        valueWidget->getTextCb(std::move(callback));
        outerRow->add(valueWidget);
    };

    addMetaDataRow("Filename", "NA",
                   [&]() -> std::string { return getAudioSystemInfo().name; });
    addMetaDataRow("Format", "NA", [&]() -> std::string {
        return getAudioSystemInfo().format;
    });
    addMetaDataRow("Sample Rate", "NA", [&]() -> std::string {
        return std::to_string(getAudioSystemInfo().sample_rate);
    });
    addMetaDataRow("Channels", "NA", [&]() -> std::string {
        return std::to_string(getAudioSystemInfo().channels);
    });
    addMetaDataRow("Total Frames", "NA", [&]() -> std::string {
        return std::to_string(getAudioSystemInfo().total_frames);
    });
    addMetaDataRow("Bitrate(kbps)", "NA", [&]() -> std::string {
        return std::to_string(getAudioSystemInfo().bitrate);
    });

    /// @brief Statistical and Analysis Row
    auto statRow = std::make_shared<Row>();
    statRow->parent(mainCol.get())->heightRel(0.3);
    mainCol->add(statRow);

    auto barPlot = std::make_shared<BarPlot>();
    barPlot->parent(statRow.get())->heightRel(1.0)->widthRel(0.5);
    barPlot->title("Binned Waveform @ Bin-size: 32 & Chunk-size: 1024");
    barPlot->minY(-1.0);
    barPlot->maxY(1.0);
    barPlot->binWidth(1);
    barPlot->color(COLOR_GREEN);
    barPlot->bgColor(COLOR_BLACK);
    barPlot->nBins(32);
    barPlot->acquireDataCb(std::move(acquireChannelDataCallback));
    statRow->add(barPlot);

    auto specPlot = std::make_shared<BarPlot>();
    specPlot->parent(statRow.get())->heightRel(1.0)->widthRel(0.5);
    // TODO: Handle the raw values, change to class properties instead
    specPlot->title("Binned Spectrum @ Bin-size: 32 & Chunk-size: 1024");
    specPlot->minY(0.0);
    specPlot->maxY(10.24);
    specPlot->binWidth(1);
    specPlot->color(COLOR_RED);
    specPlot->bgColor(COLOR_BLACK);
    specPlot->nBins(32);
    specPlot->acquireDataCb(std::move(acquireSpecDataCallback));
    statRow->add(specPlot);

    /// @brief Progress Bar Column
    auto pbCol = std::make_shared<Column>();
    pbCol->parent(mainCol.get())->height(5)->widthRel(0.9);
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
    pbTextRow->mainAxisAlignment(MX_SPACE_BETWEEN);
    pbCol->add(pbTextRow);

    auto textCurTime = std::make_shared<TextBox>();
    textCurTime->parent(pbTextRow.get())->height(1)->width(20);
    textCurTime->color(COLOR_RED);
    textCurTime->bgColor(COLOR_BLACK);
    textCurTime->getTextCb(std::move(getElapsedSecString));
    pbTextRow->add(textCurTime);

    auto textTotTime = std::make_shared<TextBox>();
    textTotTime->parent(pbTextRow.get())->height(1)->width(20);
    textTotTime->color(COLOR_GREEN);
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
    btnPrev10s->triggerKey('b');
    btnPrev10s->onClick([this](bool _) { prevSecCallback(10); });
    btnPrev10s->getStatusCb([]() -> bool { return false; });
    btnPrev10s->color(COLOR_RED);
    btnRow->add(btnPrev10s);

    auto btnPlay = std::make_shared<Button>();
    btnPlay->parent(mainCol.get())->height(3)->width(10);
    btnPlay->activeText("Pause");
    btnPlay->inactiveText("Play");
    btnPlay->triggerKey(' ');
    btnPlay->onClick(std::move(setPlaybackCallback));
    btnPlay->getStatusCb(std::move(getPlaybackCallback));
    btnRow->add(btnPlay);

    auto btnNext10s = std::make_shared<Button>();
    btnNext10s->parent(mainCol.get())->height(3)->width(10);
    btnNext10s->activeText("*");
    btnNext10s->inactiveText("10s >>");
    btnNext10s->triggerKey('f');
    btnNext10s->onClick([this](bool _) { nextSecCallback(10); });
    btnNext10s->getStatusCb([]() -> bool { return false; });
    btnNext10s->color(COLOR_RED);
    btnRow->add(btnNext10s);

    auto btnMute = std::make_shared<Button>();
    btnMute->parent(btnRow.get())->height(3)->width(10);
    btnMute->activeText("Unmute");
    btnMute->inactiveText("Mute");
    btnMute->triggerKey('m');
    btnMute->onClick(std::move(setMuteCallback));
    btnMute->getStatusCb(std::move(getMuteCallback));
    btnRow->add(btnMute);

    auto pbVolume = std::make_shared<ProgressBar>();
    pbVolume->parent(btnRow.get())->height(1)->width(16);
    pbVolume->color(COLOR_RED);
    pbVolume->bgColor(COLOR_BLACK);
    pbVolume->onTouch([this](double v) { setVolumeCallback(v); });
    pbVolume->getProgressCb([this]() -> double { return getVolumeCallback(); });
    btnRow->add(pbVolume);

    auto getVolumeString = [this]() -> std::string {
        double v = getVolumeCallback() * 100.0;
        char buff[32];
        sprintf(buff, "%.2f %%", v);
        return buff;
    };

    auto textVolume = std::make_shared<TextBox>();
    textVolume->parent(btnRow.get())->height(1)->width(12);
    textVolume->color(COLOR_GREEN);
    textVolume->bgColor(COLOR_BLACK);
    textVolume->getTextCb(std::move(getVolumeString));
    btnRow->add(textVolume);

    mainWindow->resize();
}

void CursedLayout::resize() { mainWindow->resize(); }

void CursedLayout::run() { mainWindow->run(); }
