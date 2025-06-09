#include <cstring>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <deque>
#include <complex>
#include <chrono>
#include <filesystem>


#include "AudioQueue.h"
#include "AudioSystem.h"
#include "CursedLayout.h"
#include "features/FFT.h"

using namespace std;

// --- Utility ---
std::string formatDuration(const char* prefix, double seconds) {
    char buff[48];
    sprintf(buff, "%s %.2f", prefix, seconds);
    return buff;
}

// --- Main ---
int main(int argc, char **argv) {
    if (argc < 2 || std::string(argv[1]) == "--help") {
        std::cout << "Usage: ./caf <audio_file> [chunk_size]\n"
                  << "  <audio_file>    Path to the audio file (required)\n"
                  << "  [chunk_size]    Optional: override default chunk size (default = 8192)\n";
        return 0;
    }

    std::string filename = argv[1];

    if (!std::filesystem::exists(filename)) {
        std::cerr << "Error: File does not exist -> " << filename << std::endl;
        return 1;
    }

    size_t chunkSize = 8192;  // Default chunk size
    if (argc >= 3) {
        try {
            chunkSize = std::stoul(argv[2]);
            if (chunkSize == 0 || (chunkSize & (chunkSize - 1)) != 0) {
                throw std::invalid_argument("Chunk size must be a power of 2 and > 0");
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid chunk size: " << argv[2] << "\n"
                      << "Reason: " << e.what() << "\n";
            return 1;
        }
    }

    AudioSystem audioSystem(filename, chunkSize);
    FFT<float> fft(audioSystem.getChunkSize());

    // Topics and Queues
    Topic<float*, size_t> ampTopic("amplitude");
    Topic<std::complex<float>*, size_t> specTopic("spectrum");

    AudioQueue<float> ampQueue(32, audioSystem.getChunkSize() / 32);
    AudioQueue<std::complex<float>> specQueue(32, audioSystem.getChunkSize() / 32);
    AudioQueue<float> hexQueue(48, audioSystem.getChunkSize() / 48);

    // --- Data Flow Setup ---
    ampTopic.subscribe([&](float* buffer, size_t frames) {
        ampQueue.pushBuffer(buffer, frames);
        hexQueue.pushBuffer(buffer, frames);
        fft.fill(buffer, frames);
        fft.compute();
        fft.publishResult();
    });

    specTopic.subscribe([&](std::complex<float>* buffer, size_t frames) {
        specQueue.pushBuffer(buffer, frames);
    });

    audioSystem.subscribe(ampTopic);
    fft.subscribe(specTopic);

    // --- UI Callback Setup ---
    auto playbackToggle = [&](bool play) { audioSystem.setIsPlaying(play); };
    auto setSeekRatio = [&](double ratio) {
        auto totalMs = audioSystem.getTotalDurationMs().count();
        audioSystem.seek(std::chrono::milliseconds(static_cast<long long>(totalMs * ratio)));
    };
    auto getProgressRatio = [&]() {
        return static_cast<double>(audioSystem.getCurrentFrame()) / audioSystem.getTotalFrames();
    };

    auto seekRelative = [&](int seconds, bool forward) {
        auto currentMs = audioSystem.getElapsedTimeMs().count();
        auto totalMs = audioSystem.getTotalDurationMs().count();
        long long target = forward
            ? std::min(currentMs + seconds * 1000L, totalMs)
            : std::max(currentMs - seconds * 1000L, 0L);
        audioSystem.seek(std::chrono::milliseconds(target));
    };

    // Volume and Playback Controls
    auto setVolume = [&](double v) { audioSystem.setVolume(v); };
    auto getVolume = [&]() { return audioSystem.getVolume(); };
    auto setMute = [&](bool mute) { audioSystem.setIsMute(mute); };
    auto getMute = [&]() { return audioSystem.getIsMute(); };
    auto setSpeed = [&](double speed) { audioSystem.setSpeed(speed); };
    auto getSpeed = [&]() { return audioSystem.getSpeed(); };
    auto isPlaying = [&]() { return audioSystem.getIsPlaying(); };

    // Time Display
    auto getElapsedStr = [&]() {
        return formatDuration("ELA(s)", audioSystem.getElapsedTimeMs().count() / 1000.0);
    };

    auto getTotalStr = [&]() {
        return formatDuration("TOT(s)", audioSystem.getTotalDurationMs().count() / 1000.0);
    };

    // Spectrum and Amplitude Data
    auto acquireAmplitude = [&](float* out) {
        ampQueue.fullPeek(out);
    };

    auto acquireSpectrum = [&](float* out) {
        static std::vector<std::complex<float>> buffer(specQueue.size());
        specQueue.fullPeek(buffer.data());
        std::transform(buffer.begin(), buffer.end(), out, [](const std::complex<float>& c) {
            return std::abs(c);
        });
    };

    auto audioInfo = [&]() -> AudioSystemInfo {
        return audioSystem.getInfo();
    };

    // Hex Stream Viewer
    auto hexStreamData = [&]() -> std::deque<std::pair<size_t, uint32_t>> {
        static int count = 0;
        static std::deque<std::pair<size_t, uint32_t>> buffer;

        if (!audioSystem.getIsPlaying()) return buffer;

        for (auto& f : hexQueue.peek()) {
            uint32_t asInt;
            std::memcpy(&asInt, &f, sizeof(float));
            buffer.emplace_back(count++, asInt);
            if (buffer.size() > hexQueue.size()) buffer.pop_front();
        }

        return buffer;
    };

    // --- Launch UI ---
    CursedLayout layout{
        setSeekRatio,
        playbackToggle,
        getProgressRatio,
        isPlaying,
        [&](int sec) { seekRelative(sec, false); },
        [&](int sec) { seekRelative(sec, true); },
        setVolume,
        getVolume,
        setMute,
        getMute,
        setSpeed,
        getSpeed,
        getElapsedStr,
        getTotalStr,
        acquireAmplitude,
        acquireSpectrum,
        audioInfo,
        hexStreamData
    };

    layout.mount();
    layout.run();

    return 0;
}
