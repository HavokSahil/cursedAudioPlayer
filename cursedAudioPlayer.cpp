#include <iostream>

#include "AudioSystem.h"
#include "CursedLayout.h"

using namespace std;

int main() {
    std::string filename = "/home/havoksahil/Projects/cursedAudioPlayer/tests/media/imperial_march.wav";
    AudioSystem audioSystem(filename, 8192);

    auto playbackCallback = [&audioSystem](bool value) ->void { audioSystem.setIsPlaying(value); };

    auto setTimeCallback = [&audioSystem](double timeRatio)->void {
        auto totalDuration = static_cast<long long>(audioSystem.getTotalDurationMs().count() * timeRatio);
        auto setTime = std::chrono::milliseconds(totalDuration);
        audioSystem.seek(setTime);
    };

    auto askProgress = [&audioSystem]() ->double {
        auto progress = static_cast<double>(audioSystem.getCurrentFrame()) / static_cast<double>(audioSystem.getTotalFrames());
        return progress;
    };

    auto prevSec = [&audioSystem](int seconds) -> void {
        auto currentDuration = audioSystem.getElapsedTimeMs().count();
        auto finalDuration = std::max(currentDuration - seconds * 1000, 0l);
        audioSystem.seek(std::chrono::milliseconds{finalDuration});
    };

    auto nextSec = [&audioSystem](int seconds) -> void {
        auto totalDuration = audioSystem.getTotalDurationMs().count();
        auto currentDuration = audioSystem.getElapsedTimeMs().count();
        auto finalDuration = std::min(currentDuration + seconds * 1000, totalDuration);
        audioSystem.seek(std::chrono::milliseconds{finalDuration});
    };

    auto setVolume = [&audioSystem](int volume) -> void {
        audioSystem.setVolume(volume);
    };

    auto getVolume = [&audioSystem]() -> int {
        return audioSystem.getVolume();
    };

    auto getCurrentDurString = [&audioSystem]() -> std::string {
        auto durationMs = static_cast<double>(audioSystem.getElapsedTimeMs().count()) / 1000.0;
        char buff[48];
        sprintf(buff, "ELA(s) * %.2f", durationMs);
        return buff;
    };

    auto getMaxDurString = [&audioSystem]() -> std::string {
        auto durationMs = static_cast<double>(audioSystem.getTotalDurationMs().count()) / 1000.0;
        char buff[48];
        sprintf(buff, "TOT(s) * %.2f", durationMs);
        return buff;
    };

    auto askPlayback = [&audioSystem]() ->bool { return audioSystem.getIsPlaying(); };

    CursedLayout layout{
        std::move(setTimeCallback),
        std::move(playbackCallback),
        std::move(askProgress),
        std::move(askPlayback),
        std::move(prevSec),
        std::move(nextSec),
        std::move(setVolume),
        std::move(getVolume),
        std::move(getCurrentDurString),
        std::move(getMaxDurString)
    };
    layout.mount();
    layout.run();
    return 0;
}