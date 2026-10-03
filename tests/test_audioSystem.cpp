#include <cassert>
#include <cmath>
#include <thread>

#include "AudioSystem.h"

int test_audio_system() {
    std::string filename = std::string(TEST_MEDIA_DIR) + "/imperial_march.wav";
    AudioSystem audioSystem(filename, 64);
    const auto info = audioSystem.getInfo();
    assert(info.sample_rate > 0 && info.channels > 0 && info.total_frames > 10);
    assert(audioSystem.getChunkSize() == 64);
    assert(std::abs(info.bitrate - info.sample_rate * info.channels * 32.0 / 1000) < 0.01);

    audioSystem.setVolume(2.0);
    assert(audioSystem.getVolume() == 1.0);
    audioSystem.setVolume(-1.0);
    assert(audioSystem.getVolume() == 0.0);
    audioSystem.setIsMute(true);
    assert(audioSystem.getIsMute());

    // A short final buffer must reach the device, even below its callback size.
    audioSystem.seek(info.total_frames - 10);
    audioSystem.setIsPlaying(true);
    audioSystem.setIsPlaying(true);  // Repeated play is safe.
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (audioSystem.getIsPlaying() && std::chrono::steady_clock::now() < deadline)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    assert(!audioSystem.getIsPlaying());
    assert(audioSystem.getCurrentFrame() == info.total_frames);

    audioSystem.seek(info.total_frames / 2);
    assert(audioSystem.getCurrentFrame() == info.total_frames / 2);
    audioSystem.setIsPlaying(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    audioSystem.setIsPlaying(false);
    const size_t pausedFrame = audioSystem.getCurrentFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    assert(audioSystem.getCurrentFrame() == pausedFrame);
    audioSystem.setIsPlaying(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    audioSystem.setIsPlaying(false);
    assert(audioSystem.getCurrentFrame() > pausedFrame);
    assert(audioSystem.getCurrentFrame() - pausedFrame < info.sample_rate / 2);

    audioSystem.seek(info.durationMs);
    assert(audioSystem.getCurrentFrame() == info.total_frames);
    audioSystem.reset();
    assert(audioSystem.getCurrentFrame() == 0);
    return 0;
}
