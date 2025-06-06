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

    auto askPlayback = [&audioSystem]() ->bool { return audioSystem.getIsPlaying(); };

    CursedLayout layout{
        std::move(setTimeCallback),
        std::move(playbackCallback),
        std::move(askProgress),
        std::move(askPlayback)
    };
    layout.mount();
    layout.run();
    return 0;
}