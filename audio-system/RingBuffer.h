/**
* @file RingBuffer.h
 * @brief Header for RingBuffer
 */

#pragma once

#include <err_codes.h>
#include <mutex>
#include <memory>
#include <cstring>

/// @class RingBuffer
class RingBuffer {
    std::mutex _mutex;
    /// @brief Pointer for float buffer.
    std::unique_ptr<float[]> pBuffer;
    /// @brief Number of channels.
    size_t _channels;
    /// @brief Number of frames.
    size_t _frames;
    /// @brief Position of write cursor.
    size_t _head;
    /// @brief Position of read cursor.
    size_t _tail;

public:
    /// @brief Default Constructor
    /// @param channels The number of channels in a sample.
    /// @param frames Number of frames, the buffer can store.
    RingBuffer(int channels, int frames);
    ~RingBuffer() = default;

    /// @brief Try to write the specified frames into the buffer.
    /// @param in Pointer to the input data.
    /// @param frameCount The number of frames to write.
    /// @param framesWritten The reference to the frames read variable.
    /// @returns The number of frames actually written.
    Err write(const float* in, size_t frameCount, size_t &framesWritten);

    /// @brief Try to read the specified number of frames from the buffer.
    /// @param out Pointer to the output buffer.
    /// @param frameCount Number of frames to read.
    /// @param framesRead Number of frames actually read.
    /// @returns Error code of the operation.
    Err read(float* out, size_t frameCount, size_t &framesRead);

    /// @brief Try to read the specified number of frames from the buffer without updating the read cursor.
    Err readQuiet(float* out, size_t frameCount, size_t &framesRead);

    /// @returns Number of frames available in the buffer.
    size_t framesInBuffer() const;

    /// @returns True if the buffer is empty.
    bool empty() const;

    /// @returns True if number of frames in buffer is capacity - 1.
    bool full() const;

    /// @returns The size of the ring buffer (in Frames)
    size_t size() const;

    /// @brief Resets the buffer's read/write pointer
    void reset();
};
