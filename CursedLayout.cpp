#include "CursedLayout.h"
#include <algorithm>
#include <cstdio>
#include <filesystem>

#include "BarPlot.h"
#include "BoxedContainer.h"
#include "Button.h"
#include "Column.h"
#include "FHexStream.h"
#include "ProgressBar.h"
#include "Row.h"
#include "TextBox.h"

void CursedLayout::mount() {
    mainWindow = std::make_unique<MainWindow>();
    mainWindow->build();

    auto mainCol = std::make_shared<Column>();
    mainCol->parent(mainWindow.get())->widthRel(1.0);
    mainWindow->add(mainCol);

    // Header: one stable identity, one live transport state.
    auto header = std::make_shared<Row>();
    header->height(1)->widthRel(1.0);
    header->mainAxisAlignment(MX_SPACE_BETWEEN);
    mainCol->add(header);

    auto title = std::make_shared<TextBox>();
    title->height(1)->width(28);
    title->text("CURSEDAP :: AUDIO CONSOLE");
    header->add(title);

    auto status = std::make_shared<TextBox>();
    status->height(1)->width(24);
    status->getTextCb([this]() {
        return std::string(getPlaybackCallback() ? "[ RUN ]" : "[ IDLE ]") +
            (getMuteCallback() ? "  / MUTED" : "  / PCM");
    });
    header->add(status);

    // Telemetry is grouped in two inset ncurses windows.
    auto metaRow = std::make_shared<Row>();
    metaRow->height(8)->widthRel(1.0);
    metaRow->mainAxisAlignment(MX_SPACE_BETWEEN);
    mainCol->add(metaRow);

    auto hexPanel = std::make_shared<BoxedContainer>();
    hexPanel->heightRel(1.0)->widthRel(0.49);
    hexPanel->title(" PCM::HEX / BINNED ");
    metaRow->add(hexPanel);

    auto hexStream = std::make_shared<FHexStream>();
    hexStream->parent(hexPanel.get());
    hexStream->nBytes(8);
    hexStream->nLines(6);
    hexStream->bytesCb(std::move(hexDataCallback));
    hexStream->mount();
    hexPanel->add(hexStream);

    auto sourcePanel = std::make_shared<BoxedContainer>();
    sourcePanel->heightRel(1.0)->widthRel(0.49);
    sourcePanel->title(" SOURCE::PCM ");
    metaRow->add(sourcePanel);

    auto metaCol = std::make_shared<Column>();
    metaCol->parent(sourcePanel.get());
    metaCol->mainAxisAlignment(MX_START);
    sourcePanel->add(metaCol);

    auto addMetaDataRow = [&](const char* label, std::function<std::string()>&& callback) {
        auto row = std::make_shared<Row>();
        row->height(1)->widthRel(1.0);
        row->mainAxisAlignment(MX_SPACE_BETWEEN);
        metaCol->add(row);

        auto labelWidget = std::make_shared<TextBox>();
        labelWidget->height(1)->widthRel(0.3);
        labelWidget->text(label);
        row->add(labelWidget);

        auto valueWidget = std::make_shared<TextBox>();
        valueWidget->height(1)->widthRel(0.7);
        valueWidget->color(COLOR_WHITE);
        valueWidget->getTextCb(std::move(callback));
        row->add(valueWidget);
    };
    addMetaDataRow("File", [this]() { return std::filesystem::path(getAudioSystemInfo().name).filename().string(); });
    addMetaDataRow("Format", [this]() { return std::string(getAudioSystemInfo().format); });
    addMetaDataRow("Hz", [this]() { return std::to_string(getAudioSystemInfo().sample_rate); });
    addMetaDataRow("Ch", [this]() { return std::to_string(getAudioSystemInfo().channels); });
    addMetaDataRow("Frames", [this]() { return std::to_string(getAudioSystemInfo().total_frames); });
    addMetaDataRow("kbps", [this]() {
        char value[32];
        snprintf(value, sizeof(value), "%.1f", getAudioSystemInfo().bitrate);
        return std::string(value);
    });

    auto statRow = std::make_shared<Row>();
    statRow->heightRel(0.35)->widthRel(1.0);
    statRow->mainAxisAlignment(MX_SPACE_BETWEEN);
    mainCol->add(statRow);

    auto waveform = std::make_shared<BarPlot>();
    waveform->heightRel(1.0)->widthRel(0.325);
    waveform->title(" WAVE::CH0 ");
    waveform->minY(-1.0);
    waveform->maxY(1.0);
    waveform->nBins(32);
    waveform->smoothing(0.5);
    waveform->color(COLOR_GREEN);
    waveform->axisLabel([]() { return std::string(" -1.0 / 0 / +1.0 "); });
    waveform->acquireDataCb(std::move(acquireChannelDataCallback));
    statRow->add(waveform);

    auto spectrum = std::make_shared<BarPlot>();
    spectrum->heightRel(1.0)->widthRel(0.325);
    spectrum->title(" FFT::CH0 / dBFS ");
    spectrum->minY(-80.0);
    spectrum->maxY(0.0);
    spectrum->nBins(32);
    spectrum->smoothing(0.25);
    spectrum->color(COLOR_RED);
    spectrum->axisLabel([this]() {
        char value[48];
        snprintf(value, sizeof(value), " 0 Hz -> %.1f kHz ", getAudioSystemInfo().sample_rate / 2000.0);
        return std::string(value);
    });
    spectrum->acquireDataCb(std::move(acquireSpecDataCallback));
    statRow->add(spectrum);

    auto pitchClasses = std::make_shared<BarPlot>();
    pitchClasses->heightRel(1.0)->widthRel(0.325);
    pitchClasses->title(" CQT::PITCH CLASS ");
    pitchClasses->minY(-60.0);
    pitchClasses->maxY(0.0);
    pitchClasses->nBins(12);
    pitchClasses->smoothing(0.25);
    pitchClasses->color(COLOR_GREEN);
    pitchClasses->binLabels({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"});
    pitchClasses->acquireDataCb(std::move(acquireCqtDataCallback));
    statRow->add(pitchClasses);

    auto progress = std::make_shared<ProgressBar>();
    progress->height(1)->widthRel(1.0);
    progress->onTouch([this](double ratio) { setTimeCallback(ratio); });
    progress->getProgressCb([this]() { return getTimeCallback(); });
    mainCol->add(progress);

    auto timeRow = std::make_shared<Row>();
    timeRow->height(1)->widthRel(1.0);
    timeRow->mainAxisAlignment(MX_SPACE_BETWEEN);
    mainCol->add(timeRow);
    auto elapsed = std::make_shared<TextBox>();
    elapsed->height(1)->width(16);
    elapsed->getTextCb(std::move(getElapsedSecString));
    timeRow->add(elapsed);
    auto total = std::make_shared<TextBox>();
    total->height(1)->width(16);
    total->getTextCb(std::move(getTotalSecString));
    timeRow->add(total);

    auto controls = std::make_shared<Row>();
    controls->height(3)->widthRel(1.0);
    controls->spacing(1);
    mainCol->add(controls);
    auto addButton = [&](const char* off, const char* on,
                         std::function<void(bool)>&& click, std::function<bool()>&& state) {
        auto button = std::make_shared<Button>();
        button->height(3)->width(10);
        button->inactiveText(off);
        button->activeText(on);
        button->onClick(std::move(click));
        button->getStatusCb(std::move(state));
        controls->add(button);
    };
    addButton("<< 10s", "<< 10s", [this](bool) { prevSecCallback(10); }, []() { return false; });
    addButton("> PLAY", "|| PAUSE", [this](bool play) { setPlaybackCallback(play); },
              [this]() { return getPlaybackCallback(); });
    addButton("10s >>", "10s >>", [this](bool) { nextSecCallback(10); }, []() { return false; });
    addButton("MUTE", "UNMUTE", [this](bool mute) { setMuteCallback(mute); },
              [this]() { return getMuteCallback(); });

    auto volume = std::make_shared<ProgressBar>();
    volume->height(1)->width(12);
    volume->color(COLOR_GREEN);
    volume->onTouch([this](double value) { setVolumeCallback(value); });
    volume->getProgressCb([this]() { return getVolumeCallback(); });
    controls->add(volume);
    auto volumeText = std::make_shared<TextBox>();
    volumeText->height(1)->width(10);
    volumeText->getTextCb([this]() {
        char value[16];
        snprintf(value, sizeof(value), "VOL %.0f%%", getVolumeCallback() * 100.0);
        return std::string(value);
    });
    controls->add(volumeText);

    auto help = std::make_shared<TextBox>();
    help->height(1)->widthRel(1.0);
    help->text("[SPC] play [b/f] seek [+/-] vol [m] mute [q] quit");
    mainCol->add(help);

    mainWindow->keyCb([this](int ch) {
        if (ch == ' ') setPlaybackCallback(!getPlaybackCallback());
        else if (ch == 'b' || ch == KEY_LEFT) prevSecCallback(10);
        else if (ch == 'f' || ch == KEY_RIGHT) nextSecCallback(10);
        else if (ch == 'm') setMuteCallback(!getMuteCallback());
        else if (ch == '+' || ch == '=' || ch == KEY_UP)
            setVolumeCallback(std::min(1.0, getVolumeCallback() + 0.05));
        else if (ch == '-' || ch == KEY_DOWN)
            setVolumeCallback(std::max(0.0, getVolumeCallback() - 0.05));
        else if (ch == KEY_HOME) setTimeCallback(0.0);
        else if (ch == KEY_END) setTimeCallback(1.0);
    });
    mainWindow->resize();
}

void CursedLayout::resize() { mainWindow->resize(); }
void CursedLayout::run() { mainWindow->run(); }
