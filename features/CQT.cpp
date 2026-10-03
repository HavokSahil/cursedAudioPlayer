#include "CQT.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

CQT::CQT(size_t sampleRate, float minFrequency, int binsPerOctave):
    _backend(nullptr, cap_ecqt_destroy),
    _minFrequency(minFrequency),
    _binsPerOctave(binsPerOctave),
    _hopSize(std::max<size_t>(1024, sampleRate / 40))
{
    if (sampleRate == 0 || !std::isfinite(minFrequency) || minFrequency <= 0 ||
        minFrequency >= sampleRate / 2.0 || binsPerOctave < 1 || binsPerOctave > 48)
        throw std::invalid_argument("Invalid CQT frequency configuration");
    _backend.reset(cap_ecqt_create(sampleRate, minFrequency, binsPerOctave));
    if (!_backend) throw std::runtime_error("Failed to initialize ECQT analysis");
    _samples.resize(cap_ecqt_length(_backend.get()), 0.0f);
    _magnitudes.resize(cap_ecqt_bins(_backend.get()), 0.0f);
}

double CQT::getBinFrequency(size_t bin) const {
    if (bin >= getBinCount()) throw std::out_of_range("CQT bin out of range");
    return _minFrequency * std::pow(2.0, static_cast<double>(bin) / _binsPerOctave);
}

bool CQT::process(const float* samples, size_t count) {
    if (!samples || count == 0) return false;
    std::lock_guard lock(_mutex);
    const size_t copyCount = std::min(count, _samples.size());
    std::move(_samples.begin() + copyCount, _samples.end(), _samples.begin());
    std::copy_n(samples + count - copyCount, copyCount, _samples.end() - copyCount);
    _pending += count;
    if (_pending < _hopSize) return false;
    _pending %= _hopSize;
    if (cap_ecqt_transform(_backend.get(), _samples.data(), _samples.size(), _magnitudes.data()) != 0)
        throw std::runtime_error("ECQT transform failed");
    this->publish(_magnitudes.data(), _magnitudes.size());
    return true;
}

void CQT::reset() {
    std::lock_guard lock(_mutex);
    std::fill(_samples.begin(), _samples.end(), 0.0f);
    std::fill(_magnitudes.begin(), _magnitudes.end(), 0.0f);
    _pending = 0;
}

void CQT::foldPitchClasses(const float* magnitudes, size_t count, float minFrequency,
                           float* output, int binsPerOctave) {
    if (!magnitudes || !output || !std::isfinite(minFrequency) || minFrequency <= 0 || binsPerOctave < 1)
        throw std::invalid_argument("Invalid pitch-class configuration");
    double energy[12]{};
    const long firstNote = std::lround(69 + 12 * std::log2(minFrequency / 440.0));
    for (size_t bin = 0; bin < count; ++bin) {
        const float magnitude = magnitudes[bin];
        if (!std::isfinite(magnitude) || magnitude <= 0) continue;
        const long note = firstNote + std::lround(12.0 * bin / binsPerOctave);
        const size_t pitchClass = (note % 12 + 12) % 12;
        energy[pitchClass] += static_cast<double>(magnitude) * magnitude;
    }
    const double peak = *std::max_element(energy, energy + 12);
    for (size_t i = 0; i < 12; ++i)
        output[i] = peak > 1e-8 ? std::sqrt(energy[i] / peak) : 0.0f;
}
