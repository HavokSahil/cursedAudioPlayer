/**
 * @file RingBuffer.cpp
 * @brief Module file for RingBuffer Implementation
 */

#include "RingBuffer.h"

RingBuffer::RingBuffer(const int channels, const int frames):
    _channels(channels),
    _frames(frames),
    _head(0),
    _tail(0)
{
    const size_t bufferSize = (_frames + 1) * _channels + 1;
    this->pBuffer = std::make_unique<float[]>(bufferSize);
}

Err RingBuffer::write(const float *in, const size_t frameCount, size_t &framesWritten) {
    std::unique_lock lock(_mutex);
    const size_t framesToWrite = std::min(_frames - framesInBuffer(), frameCount);
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
    const size_t available = std::min(frameCount, framesInBuffer());
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
    // const size_t remaining = frameCount - available;    // Pad the remaining space with zero
    // memset(out + available * _channels, 0, remaining * _channels * sizeof(float));
    framesRead = available;
    return ERR_OK;
}

Err RingBuffer::readQuiet(float *out, const size_t frameCount, size_t &framesRead) {    // Just the read without the cursor update
    std::unique_lock lock(_mutex);
    const size_t available = std::min(frameCount, framesInBuffer());
    if (_tail + available > _frames) {
        const size_t szEnd = _frames - _tail + 1;
        memcpy(out, pBuffer.get() + _tail, szEnd * _channels * sizeof(float));
        const size_t szStart = available - szEnd;
        memcpy(out + szEnd, pBuffer.get(), szStart * _channels * sizeof(float));
    } else {
        memcpy(out, pBuffer.get() + _tail, available * _channels * sizeof(float));
    }
    // const size_t remaining = frameCount - available;    // Pad the remaining space with zero
    // memset(out, 0, remaining * _channels * sizeof(float));

    framesRead = available;
    return ERR_OK;
}

size_t RingBuffer::framesInBuffer() const {
    return (_head < _tail) ?
        _frames - _tail + _head + 1:
        _head - _tail;
}

bool RingBuffer::empty() const {
    return _head == _tail;
}

bool RingBuffer::full() const {
    return framesInBuffer() == _frames;
}

size_t RingBuffer::size() const {
    return _frames;
}

void RingBuffer::reset() {
    _head = _tail = 0;
}
