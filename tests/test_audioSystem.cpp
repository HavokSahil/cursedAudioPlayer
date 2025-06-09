#include <cassert>

#include "AudioSystem.h"
#include "RingBuffer.h"
#include "AudioPlayer.h"
#include "AudioDecoder.h"

int test_audio_system() {
    std::string filename = "/home/havoksahil/Projects/cursedAudioPlayer/tests/media/music.mp3";
    constexpr size_t bufferSize = 8192;
    LOGI("Starting audio system test.....");

    AudioSystem audioSystem(filename, bufferSize);

    const AudioSystemInfo info = audioSystem.getInfo();

    LOGI("Audio system info:");
    LOGI("Name: %s", info.name);
    LOGI("Description: %s", info.description);
    LOGI("Format: %s", info.format);
    LOGI("Sample Rate: %lld", info.sample_rate);
    LOGI("Channels: %lld", info.channels);
    LOGI("Bitrate (kbps): %.2f", info.bitrate);
    LOGI("Duration (ms): %lld", info.durationMs.count());

    assert(info.sample_rate > 0);
    assert(info.channels > 0);
    assert(info.bitrate > 0.0);
    assert(info.durationMs.count() > 0);

    audioSystem.setIsPlaying(true);

    while (audioSystem.getIsPlaying()) {
        LOGI("Waiting for audio stream to play...");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return ERR_OK;
}