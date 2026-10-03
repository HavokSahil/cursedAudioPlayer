#include <cstring>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <deque>
#include <complex>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>


#include "AudioQueue.h"
#include "AudioSystem.h"
#include "CursedLayout.h"
#include "features/FFT.h"
#include "features/CQT.h"
#include <mutex>

using namespace std;

// --- Utility ---
std::string formatDuration(const char* prefix, double seconds) {
    const auto time = static_cast<long long>(seconds);
    std::ostringstream out;
    out << prefix << " " << time / 60 << ':' << std::setfill('0') << std::setw(2) << time % 60;
    return out.str();
}

// --- Main ---
int runPlayer(int argc, char **argv) {
    if (argc < 2 || std::string(argv[1]) == "--help") {
        std::cout << "Usage: cursedap <audio_file> [chunk_size]\n"
                  << "  <audio_file>    Path to the audio file (required)\n"
                  << "  [chunk_size]    Optional: override default chunk size (default = 1024)\n";
        return 0;
    }

    std::string filename = argv[1];

    if (!std::filesystem::exists(filename)) {
        std::cerr << "Error: File does not exist -> " << filename << std::endl;
        return 1;
    }

    size_t chunkSize = 1024;  // Default chunk size
    if (argc >= 3) {
        try {
            const std::string value = argv[2];
            size_t parsed = 0;
            if (value.empty() || value.front() == '-') throw std::invalid_argument("Invalid chunk size");
            chunkSize = std::stoul(value, &parsed);
            if (parsed != value.size()) throw std::invalid_argument("Unexpected characters in chunk size");
            if (chunkSize < 64 || chunkSize > (1U << 20) || (chunkSize & (chunkSize - 1)) != 0) {
                throw std::invalid_argument("Chunk size must be a power of 2 between 64 and 1048576");
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid chunk size: " << argv[2] << "\n"
                      << "Reason: " << e.what() << "\n";
            return 1;
        }
    }

    AudioSystem audioSystem(filename, chunkSize);
    FFT<float> fft(audioSystem.getChunkSize());
    CQT cqt(audioSystem.getInfo().sample_rate, std::min(55.0f, audioSystem.getInfo().sample_rate / 8.0f));
    std::vector<float> cqtData(12, 0.0f);
    std::mutex cqtMutex;

    // Topics and Queues
    std::vector<float> spectrum(audioSystem.getChunkSize() / 2);
    std::vector<float> window(audioSystem.getChunkSize());
    Topic<float*, size_t> ampTopic("amplitude");
    Topic<std::complex<float>*, size_t> specTopic("spectrum");
    Topic<float*, size_t> cqtTopic("constant-q");

    AudioQueue<float> ampQueue(32, audioSystem.getChunkSize() / 32);
    AudioQueue<float> specQueue(32, audioSystem.getChunkSize() / 64);
    AudioQueue<float> hexQueue(12, audioSystem.getChunkSize() / 12);
    struct PlaybackGuard {
        AudioSystem& system;
        ~PlaybackGuard() { system.setIsPlaying(false); }
    } playbackGuard{audioSystem};

    // --- Data Flow Setup ---
    ampTopic.subscribe([&](float* buffer, size_t frames) {
        ampQueue.pushBuffer(buffer, frames);
        hexQueue.pushBuffer(buffer, frames);
        for (size_t i = 0; i < window.size(); ++i) {
            const float sample = i < frames ? buffer[i] : 0.0f;
            window[i] = sample * 0.5f * (1.0f - std::cos(2.0f * std::numbers::pi_v<float> * i / (window.size() - 1)));
        }
        fft.fill(window.data(), window.size());
        fft.compute();
        fft.publishResult();
        cqt.process(buffer, frames);
    });

    specTopic.subscribe([&](std::complex<float>* buffer, size_t frames) {
        const size_t bins = std::min(spectrum.size(), frames / 2);
        for (size_t i = 0; i < bins; ++i)
            spectrum[i] = std::abs(buffer[i]) * 4.0f / audioSystem.getChunkSize();
        specQueue.pushBuffer(spectrum.data(), bins);
    });

    cqtTopic.subscribe([&](float* data, size_t bins) {
        std::lock_guard lock(cqtMutex);
        CQT::foldPitchClasses(data, bins, cqt.getMinFrequency(), cqtData.data(), cqt.getBinsPerOctave());
    });
    cqt.subscribe(cqtTopic);

    audioSystem.subscribe(ampTopic);
    fft.subscribe(specTopic);

    auto resetVisuals = [&]() {
        cqt.reset();
        ampQueue.reset();
        specQueue.reset();
        hexQueue.reset();
        std::lock_guard lock(cqtMutex);
        std::fill(cqtData.begin(), cqtData.end(), 0.0f);
    };

    // --- UI Callback Setup ---
    auto playbackToggle = [&](bool play) {
        if (play && audioSystem.getCurrentFrame() == audioSystem.getTotalFrames()) resetVisuals();
        audioSystem.setIsPlaying(play);
    };
    auto setSeekRatio = [&](double ratio) {
        auto totalMs = audioSystem.getTotalDurationMs().count();
        audioSystem.seek(std::chrono::milliseconds(static_cast<long long>(totalMs * ratio)));
        resetVisuals();
    };
    auto getProgressRatio = [&]() {
        const auto total = audioSystem.getTotalFrames();
        return total ? static_cast<double>(audioSystem.getCurrentFrame()) / total : 0.0;
    };

    auto seekRelative = [&](int seconds, bool forward) {
        auto currentMs = audioSystem.getElapsedTimeMs().count();
        auto totalMs = audioSystem.getTotalDurationMs().count();
        long long target = forward
            ? std::min(currentMs + seconds * 1000L, totalMs)
            : std::max(currentMs - seconds * 1000L, 0L);
        audioSystem.seek(std::chrono::milliseconds(target));
        resetVisuals();
    };

    // Volume and Playback Controls
    auto setVolume = [&](double v) { audioSystem.setVolume(v); };
    auto getVolume = [&]() { return audioSystem.getVolume(); };
    auto setMute = [&](bool mute) { audioSystem.setIsMute(mute); };
    auto getMute = [&]() { return audioSystem.getIsMute(); };
    auto isPlaying = [&]() { return audioSystem.getIsPlaying(); };

    // Time Display
    auto getElapsedStr = [&]() {
        return formatDuration("POS", audioSystem.getElapsedTimeMs().count() / 1000.0);
    };

    auto getTotalStr = [&]() {
        return formatDuration("LEN", audioSystem.getTotalDurationMs().count() / 1000.0);
    };

    // Spectrum and Amplitude Data
    auto acquireAmplitude = [&](float* out) {
        ampQueue.fullPeek(out);
    };

    auto acquireSpectrum = [&](float* out) {
        specQueue.fullPeek(out);
        for (size_t i = 0; i < specQueue.size(); ++i)
            out[i] = 20.0f * std::log10(std::max(out[i], 0.0001f));
    };

    auto acquireCqt = [&](float* out) {
        std::lock_guard lock(cqtMutex);
        for (size_t i = 0; i < cqtData.size(); ++i)
            out[i] = 20.0f * std::log10(std::max(cqtData[i], 0.0001f));
    };

    auto audioInfo = [&]() -> AudioSystemInfo {
        return audioSystem.getInfo();
    };

    // Hex Stream Viewer
    auto hexStreamData = [&]() -> std::deque<std::pair<size_t, uint32_t>> {
        std::deque<std::pair<size_t, uint32_t>> buffer;
        const auto samples = hexQueue.peek();
        const size_t current = audioSystem.getCurrentFrame();
        const size_t stride = audioSystem.getChunkSize() / 12;
        const size_t first = current > samples.size() * stride ? current - samples.size() * stride : 0;
        size_t index = 0;
        for (const auto sample : samples) {
            uint32_t asInt;
            std::memcpy(&asInt, &sample, sizeof(float));
            buffer.emplace_back(first + index++ * stride, asInt);
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
        getElapsedStr,
        getTotalStr,
        acquireAmplitude,
        acquireSpectrum,
        acquireCqt,
        audioInfo,
        hexStreamData
    };

    layout.mount();
    layout.run();

    return 0;
}

int main(int argc, char **argv) {
    try {
        return runPlayer(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "cursedap: " << e.what() << '\n';
        return 1;
    }
}
