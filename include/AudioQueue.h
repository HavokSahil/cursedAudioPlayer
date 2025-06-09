#pragma once

#include <vector>
#include <deque>
#include <mutex>

static size_t gbinSizeCounter = 0;

template<typename I>
class AudioQueue {

public:
    AudioQueue(size_t capacity, size_t binSize)
        : _capacity(capacity),
          _binSize(binSize),
          _queue(std::deque<I>(capacity, I(0))) {}

    void pushBuffer(I* buffer, size_t frames) {
        std::lock_guard<std::mutex> lock(_mutex);
        I cumVal = I(0);
        for (size_t i = 0; i < frames; ++i) {
            cumVal += buffer[i];
            gbinSizeCounter++;
            if (gbinSizeCounter % _binSize == 0) {
                pushPointUnlocked(cumVal / static_cast<I>(_binSize));
                gbinSizeCounter = 0;
            }
        }
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

    size_t _binSize{16};
    size_t _capacity;
    mutable std::mutex _mutex;
    std::deque<I> _queue;
};
