/**
 * @file RingBuffer.cpp
 * @brief Module file for RingBuffer Implementation
 */

#include "RingBuffer.h"
#include <algorithm>
#include <stdexcept>

RingBuffer::RingBuffer(const int channels, const int frames):
    _channels(channels),
    _frames(frames),
    _head(0),
    _tail(0)
{
    if (channels <= 0 || frames <= 0) throw std::invalid_argument("Invalid ring buffer size");
    const size_t bufferSize = (_frames + 1) * _channels + 1;
    this->pBuffer = std::make_unique<float[]>(bufferSize);
}

Err RingBuffer::write(const float *in, const size_t frameCount, size_t &framesWritten) {
    std::unique_lock lock(_mutex);
    const size_t framesToWrite = std::min(_frames - _framesInBuffer(), frameCount);
    if (framesToWrite + _head > _frames) {                  // a round wrap is required
        const size_t szEnd = _frames - _head + 1;           // Size of buffer till end
        memcpy(this->pBuffer.get() + _head * _channels, in, szEnd * _channels * sizeof(float));
        const size_t szStart = framesToWrite - szEnd;       // Size of buffer from start till framesToWrite
        memcpy(this->pBuffer.get(), in + szEnd * _channels, szStart * _channels * sizeof(float));
        _head = (_head + framesToWrite) % (_frames + 1);          // Update the write head with round wrap
    } else {
        memcpy(this->pBuffer.get() + _head * _channels, in, framesToWrite * _channels * sizeof(float));
        _head += framesToWrite;                             // Update the write head
    }
    framesWritten = framesToWrite;
    return ERR_OK;
}

Err RingBuffer::read(float *out, const size_t frameCount, size_t &framesRead) {
    std::unique_lock lock(_mutex);
    // If there is not enough frames; pad the data with zero
    const size_t available = std::min(frameCount, _framesInBuffer());
    if (_tail + available > _frames) {              // a round wrap is required
        const size_t szEnd = _frames - _tail + 1;   // Size till the end
        memcpy(out, pBuffer.get() + _tail * _channels, szEnd * _channels * sizeof(float));
        const size_t szStart = available - szEnd;   // Size from the beginning
        memcpy(out + szEnd * _channels, pBuffer.get(), szStart * _channels * sizeof(float));
        _tail = (_tail + available) % (_frames + 1);      // update the read head with round wrap
    } else {
        memcpy(out, pBuffer.get() + _tail * _channels, available * _channels * sizeof(float));
        _tail += available;                         // update the read head
    }
    std::fill_n(out + available * _channels, (frameCount - available) * _channels, 0.0f);
    framesRead = available;
    return ERR_OK;
}

Err RingBuffer::readQuiet(float *out, const size_t frameCount, size_t &framesRead) {    // Just the read without the cursor update
    std::unique_lock lock(_mutex);
    const size_t available = std::min(frameCount, _framesInBuffer());
    if (_tail + available > _frames) {
        const size_t szEnd = _frames - _tail + 1;
        memcpy(out, pBuffer.get() + _tail * _channels, szEnd * _channels * sizeof(float));
        const size_t szStart = available - szEnd;
        memcpy(out + szEnd * _channels, pBuffer.get(), szStart * _channels * sizeof(float));
    } else {
        memcpy(out, pBuffer.get() + _tail * _channels, available * _channels * sizeof(float));
    }
    std::fill_n(out + available * _channels, (frameCount - available) * _channels, 0.0f);

    framesRead = available;
    return ERR_OK;
}

size_t RingBuffer::framesInBuffer() const {
    std::lock_guard lock(_mutex);
    return _framesInBuffer();
}

size_t RingBuffer::_framesInBuffer() const {
    return (_head < _tail) ?
        _frames - _tail + _head + 1:
        _head - _tail;
}

bool RingBuffer::empty() const {
    std::lock_guard lock(_mutex);
    return _head == _tail;
}

bool RingBuffer::full() const {
    std::lock_guard lock(_mutex);
    return _framesInBuffer() == _frames;
}

size_t RingBuffer::size() const {
    return _frames;
}

void RingBuffer::reset() {
    std::lock_guard lock(_mutex);
    _head = _tail = 0;
}
