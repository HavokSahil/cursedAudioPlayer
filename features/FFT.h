#pragma once

#include <algorithm>
#include <complex>
#include <deque>
#include <memory>
#include <cmath>
#include <mutex>
#include <stdexcept>
#include <numbers>

#include "PublisherBase.h"

template<typename T = double>
class FFT final: public PublisherBase<std::complex<T>*, size_t> {
public:
    explicit FFT(size_t const size):
        _size(size),
        _mem(std::make_unique<std::complex<T>[]>(size))
    {
        if (size == 0 || (size & (size - 1)) != 0)
            throw std::invalid_argument("FFT size must be a power of two");
    }

    static size_t iMSB(const size_t num) {
        size_t nBits = 0;
        for (size_t i = 0; i < sizeof(size_t) * 8; i++) {
            if (num & (1ULL << i))
                nBits = std::max(nBits, i + 1);
        }
        return nBits;
    }

    static size_t _bitReverse(const size_t num, const size_t nBits) {
        size_t reverse = 0;
        for (size_t i = 0; i < nBits; i++) {
            reverse = (reverse << 1) | ((num >> i) & 1);
        }
        return reverse;
    }

    void fill(const T* buffer, const size_t len) {
        std::unique_lock lock(_mutex);
        size_t n = std::min(len, _size);
        std::transform(buffer, buffer + n, _mem.get(), [](T const& x) {
            return std::complex<T>(x, 0);
        });
        std::fill(_mem.get() + n, _mem.get() + _size, std::complex<T>(0.0, 0.0));
    }

    void fill(const std::complex<T>* buffer, const size_t len) {
        std::unique_lock lock(_mutex);
        size_t n = std::min(len, _size);
        std::copy_n(buffer, n, _mem.get());
        std::fill(_mem.get() + n, _mem.get() + _size, std::complex<T>(0.0, 0.0));
    }

    void fill(std::deque<std::complex<T>> buffer) {
        std::unique_lock lock(_mutex);
        size_t n = std::min(_size, buffer.size());
        std::copy_n(buffer.begin(), n, _mem.get());
        std::fill(_mem.get() + n, _mem.get() + _size, std::complex<T>(0, 0));
    }

    void fill(std::deque<T> buffer) {
        std::unique_lock lock(_mutex);
        size_t n = std::min(_size, buffer.size());
        std::transform(buffer.begin(), buffer.begin() + n, _mem.get(), [](const T &val) {
            return std::complex<T>(val, 0);
        });
        std::fill(_mem.get() + n, _mem.get() + _size, std::complex<T>(0.0, 0.0));
    }

    void compute(const bool inverse = false) {
        std::unique_lock lock(_mutex);
        for (size_t i = 0; i < _size; ++i) {
            const size_t j = _bitReverse(i, iMSB(_size) - 1);
            if (i < j) std::swap(_mem[i], _mem[j]);
        }

        for (size_t s = 1; (size_t(1) << s) <= _size; ++s) {
            const size_t m = size_t(1) << s;
            const std::complex<T> wm = std::polar(static_cast<T>(1.0), static_cast<T>((inverse ? 1 : -1) * 2 * std::numbers::pi / m));

            for (size_t k = 0; k < _size; k += m) {
                std::complex<T> w(1.0, 0.0);
                for (size_t j = 0; j < m / 2; ++j) {
                    auto t = w * _mem[k + j + m / 2];
                    auto u = _mem[k + j];
                    _mem[k + j] = u + t;
                    _mem[k + j + m / 2] = u - t;
                    w *= wm;
                }
            }
        }

        if (inverse) {
            for (size_t i = 0; i < _size; ++i) {
                _mem[i] /= _size;
            }
        }
    }

    void publishResult() {
        this->publish(_mem.get(), _size);
    }

private:
    std::mutex _mutex;
    size_t _size;
    std::unique_ptr<std::complex<T>[]> _mem;
};
