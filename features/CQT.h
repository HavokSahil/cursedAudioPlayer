#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "PublisherBase.h"
#include "CQTBackend.h"

// One ECQT context per analysis stream; C implementation stays behind this wrapper.
class CQT final: public PublisherBase<float*, size_t> {
public:
    explicit CQT(size_t sampleRate, float minFrequency = 55.0f, int binsPerOctave = 12);
    size_t getBinCount() const { return _magnitudes.size(); }
    size_t getWindowSize() const { return _samples.size(); }
    float getMinFrequency() const { return _minFrequency; }
    int getBinsPerOctave() const { return _binsPerOctave; }
    double getBinFrequency(size_t bin) const;

    // Keep a rolling full-length window; small audio chunks do not lose low notes.
    bool process(const float* samples, size_t count);
    void reset();
    static void foldPitchClasses(const float* magnitudes, size_t count, float minFrequency,
                                 float* output, int binsPerOctave = 12);

private:
    std::unique_ptr<CQTBackend, decltype(&cap_ecqt_destroy)> _backend;
    std::vector<float> _samples;
    std::vector<float> _magnitudes;
    std::mutex _mutex;
    float _minFrequency;
    int _binsPerOctave;
    size_t _hopSize;
    size_t _pending{0};
};
