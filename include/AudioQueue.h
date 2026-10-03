#pragma once

#include <vector>
#include <deque>
#include <mutex>
#include <algorithm>
#include <stdexcept>



template<typename I>
class AudioQueue {

public:
    AudioQueue(size_t capacity, size_t binSize)
        : _binSize(binSize),
          _capacity(capacity),
          _queue(std::deque<I>(capacity, I(0))) {
        if (capacity == 0 || binSize == 0) throw std::invalid_argument("Invalid audio queue size");
    }

    void pushBuffer(I* buffer, size_t frames) {
        std::lock_guard<std::mutex> lock(_mutex);
        for (size_t i = 0; i < frames; ++i) {
            _sum += buffer[i];
            if (++_count == _binSize) {
                pushPointUnlocked(_sum / static_cast<I>(_binSize));
                _sum = I(0);
                _count = 0;
            }
        }
    }

    void reset() {
        std::lock_guard<std::mutex> lock(_mutex);
        std::fill(_queue.begin(), _queue.end(), I(0));
        _sum = I(0);
        _count = 0;
    }

    void pushPoint(I data) {
        std::lock_guard<std::mutex> lock(_mutex);
        pushPointUnlocked(data);
    }

    void fullPeek(I* buffer) const {
        std::lock_guard<std::mutex> lock(_mutex);
        std::copy(_queue.begin(), _queue.end(), buffer);
    }

    std::deque<I> peek() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue;
    }

    size_t capacity() const { return _capacity; }
    size_t size() const { return _capacity; }

private:
    void pushPointUnlocked(I data) {
        if (_queue.size() >= _capacity) {
            _queue.pop_front();
        }
        _queue.push_back(data);
    }

    I _sum{0};
    size_t _count{0};
    size_t _binSize{16};
    size_t _capacity;
    mutable std::mutex _mutex;
    std::deque<I> _queue;
};
