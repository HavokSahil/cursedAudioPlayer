#pragma once

#include <algorithm>
#include <complex>
#include <deque>
#include <memory>
#include <vector>
#include <cmath>

#include "PublisherBase.h"

template<typename T = double>
class FFT final: public PublisherBase<std::vector<std::complex<T>>&, size_t> {
public:
    explicit FFT(size_t const size):
        _size(size)
    {
        _mem = std::vector(_size, std::complex<T>(0, 0));
    };

    static size_t iMSB(const size_t num) {
        size_t nBits = 0;
        for (size_t i = 0; i < sizeof(size_t) * 8; i++) {
            if (num & (1ULL << i))
                nBits = std::max(nBits, i + 1);
        }
        return nBits;
    }

    static size_t _bitReverse(const size_t num) {
        const size_t nBits = iMSB(num);
        size_t reverse = 0;
        for (int i = 0; i<nBits; i++) {
            reverse = (reverse << 1) | ((num >> i) & 1);
        }
        return reverse;
    }

    void fill(const T* buffer, const size_t len) {
        size_t n = std::min(len, _size);
        std::transform(buffer, buffer + n, _mem.begin(), [](T const& x) {
            return std::complex<T>(x, 0);
        });
        std::fill(_mem.begin() + n, _mem.end(), std::complex<T>(0.0, 0.0));
    }

    void fill(const std::complex<T>* buffer, const size_t len) {
        size_t n = std::min(len, _size);
        std::copy_n(buffer, n, _mem.begin());
        std::fill(_mem.begin() + n, _mem.end(), std::complex<T>(0.0, 0.0));
    }

    void fill(std::deque<std::complex<T>> buffer) {
        size_t n = std::min(_size, buffer.size());
        std::copy_n(buffer.begin(), n, _mem.begin());
        std::fill(_mem.begin() + n, _mem.end(), std::complex<T>(0, 0));
    }

    void fill(std::deque<T> buffer) {
        size_t n = std::min(_size, buffer.size());
        std::transform(buffer.begin(), buffer.begin() + n, _mem.begin(), [](const T &val) {
            return std::complex<T>(val, 0);
        });
        std::fill(_mem.begin() + n, _mem.end(), std::complex<T>(0.0, 0.0));
    }

    void compute(const bool inverse = false) {
        for (size_t i = 0; i < _size; ++i) {
            size_t j = _bitReverse(i);
            if (i < j) std::swap(_mem[i], _mem[j]);
        }

        for (size_t s = 1; (1U << s) <= _size; ++s) {
            const size_t m = 1U << s;
            const std::complex<double> wm = std::polar(1.0, (inverse ? -1 : 1) * 2 * M_PI / m);

            for (size_t k = 0; k < _size; k += m) {
                std::complex<double> w(1.0, 0.0);
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
            for (auto& x : _mem) x /= _size;
        }
    }

    void publishResult() {
        this->publish(_mem, _size);
    }

private:
    size_t _size;
    std::vector<std::complex<T>> _mem;
};
