#pragma once

#include <cstring>
#include <mutex>
#include <queue>

template<typename I, typename T = double>
class AudioQueue {
public:
    AudioQueue(const size_t capacity, const size_t channels):
        _queues(std::vector(channels, std::deque<T>(capacity, 0))),
        _capacity(capacity),
        _channels(channels)
    {}

    void pushBuffer(const I* buffer, const size_t frames) {
        size_t _ch = 0;
        for (int i = 0; i<frames * _channels; i++) {
            pushPoint(buffer[i], _ch);
            _ch = (_ch + 1) % _channels;
        }
    }

    void pushPoint(const T data, const size_t channel) {
        _queues[channel].push_back(data);
        _queues[channel].pop_front();
    }

    void fullPeek(T* buffer, const size_t _channel) const {
        memcpy(buffer, &_queues[_channel][0], _capacity);
    }

    const std::deque<T>& peek(const size_t channel) const {
        return _queues[channel];
    }

private:
    std::vector<std::deque<T>> _queues;
    size_t _capacity;
    std::mutex _mutex;
    size_t _channels;
};
