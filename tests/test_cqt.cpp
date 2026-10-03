#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>

#include "CQT.h"

int test_cqt() {
    constexpr size_t sampleRate = 22050;
    CQT cqt(sampleRate);
    std::vector<float> input(cqt.getWindowSize());
    std::vector<float> magnitudes(cqt.getBinCount());
    Topic<float*, size_t> topic("cqt-test");
    topic.subscribe([&](float* data, size_t count) {
        assert(count == magnitudes.size());
        std::copy_n(data, count, magnitudes.begin());
    });
    cqt.subscribe(topic);
    assert(cqt.getBinsPerOctave() == 12);
    assert(std::abs(cqt.getBinFrequency(12) - 110.0) < 0.001);

    // Low notes and upper-octave notes exercise the full sparse FFT kernel.
    for (const double frequency : {55.0, 110.0, 440.0, 880.0, 3520.0, 7040.0}) {
        cqt.reset();
        for (size_t i = 0; i < input.size(); ++i)
            input[i] = std::sin(2 * std::numbers::pi * frequency * i / sampleRate);
        assert(cqt.process(input.data(), input.size()));
        const auto peak = std::max_element(magnitudes.begin(), magnitudes.end());
        const auto bin = std::distance(magnitudes.begin(), peak);
        const auto expected = std::lround(12 * std::log2(frequency / 55));
        assert(std::abs(bin - expected) <= 1);
        assert(*peak > 0.8f && *peak < 1.2f);
        float classes[12];
        CQT::foldPitchClasses(magnitudes.data(), magnitudes.size(), 55.0f, classes);
        assert(std::distance(classes, std::max_element(classes, classes + 12)) == 9);
        assert(classes[9] == 1.0f);  // All these tones are A, across octaves.
    }

    for (const int pitchClass : {0, 1, 4}) {
        const double frequency = 440 * std::pow(2.0, (60 + pitchClass - 69) / 12.0);
        for (size_t i = 0; i < input.size(); ++i)
            input[i] = std::sin(2 * std::numbers::pi * frequency * i / sampleRate);
        cqt.reset();
        cqt.process(input.data(), input.size());
        float classes[12];
        CQT::foldPitchClasses(magnitudes.data(), magnitudes.size(), 55.0f, classes);
        assert(std::distance(classes, std::max_element(classes, classes + 12)) == pitchClass);
    }

    // Rolling short chunks must retain the same low-frequency resolution.
    cqt.reset();
    for (size_t i = 0; i < input.size(); ++i)
        input[i] = std::sin(2 * std::numbers::pi * 110 * i / sampleRate);
    for (size_t i = 0; i < input.size(); i += 256)
        cqt.process(input.data() + i, std::min<size_t>(256, input.size() - i));
    assert(std::distance(magnitudes.begin(), std::max_element(magnitudes.begin(), magnitudes.end())) == 12);

    cqt.reset();
    std::fill(input.begin(), input.end(), 0.0f);
    assert(cqt.process(input.data(), input.size()));
    for (const auto value : magnitudes) assert(value == 0.0f);
    assert(!cqt.process(nullptr, 0));

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 40; ++i) cqt.process(input.data(), input.size());
    const auto time = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start);
    std::cout << "ECQT: " << cqt.getBinCount() << " bins, " << cqt.getWindowSize()
              << " samples, " << time.count() / 40 << " ms/transform\n";

    for (const size_t rate : {44100, 48000, 96000, 192000}) {
        CQT alternate(rate);
        std::vector<float> silence(alternate.getWindowSize(), 0.0f);
        assert(alternate.process(silence.data(), silence.size()));
    }

    bool rejected = false;
    try { CQT invalid(sampleRate, 0); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    return 0;
}
