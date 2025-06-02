#include "RingBuffer.h"
#include <cstring>
#include <cassert>
#include <iostream>

int test_ring_buffer() {
    constexpr int channels = 2;
    constexpr int totalFrames = 1024;
    RingBuffer buffer(channels, totalFrames);

    assert(buffer.empty());
    assert(!buffer.full());

    float samplesWrite[2048]; // enough for 1024 stereo frames
    float samplesRead[2048] = {0};  // clear buffer for read

    for (int i = 0; i < 2048; ++i)
        samplesWrite[i] = static_cast<float>(i);

    // 1. Write less than capacity
    size_t framesWritten = 0;
    CHECK_ERR(buffer.write(samplesWrite, 128, framesWritten));
    assert(framesWritten == 128);
    assert(buffer.framesInBuffer() == 128);

    // 2. Read back partial
    size_t framesRead = 0;
    CHECK_ERR(buffer.read(samplesRead, 64, framesRead));
    assert(framesRead == 64);
    assert(buffer.framesInBuffer() == 64);
    for (int i = 0; i < 64 * channels; ++i) {
        assert(samplesRead[i] == samplesWrite[i]);
    }

    // 3. Read remaining
    CHECK_ERR(buffer.read(samplesRead, 128, framesRead));
    assert(framesRead == 64);
    assert(buffer.framesInBuffer() == 0);
    assert(buffer.empty());

    // 4. Fill buffer completely
    CHECK_ERR(buffer.write(samplesWrite, totalFrames, framesWritten));
    assert(framesWritten == totalFrames); // due to wrap logic

    LOGI("Frames in buffer: %lld", buffer.framesInBuffer());

    // 5. Check full status
    assert(buffer.full());

    // 6. Try writing more — should be no-op
    CHECK_ERR(buffer.write(samplesWrite, 10, framesWritten));
    assert(framesWritten == 0);

    // 7. Read half
    CHECK_ERR(buffer.read(samplesRead, totalFrames / 2, framesRead));
    assert(framesRead == totalFrames / 2);

    // 8. Write again to test wrap-around
    CHECK_ERR(buffer.write(samplesWrite, 100, framesWritten));
    assert(framesWritten == 100);

    // 9. Test readQuiet (should not advance tail)
    size_t prevFrames = buffer.framesInBuffer();
    size_t quietRead = 0;
    CHECK_ERR(buffer.readQuiet(samplesRead, 50, quietRead));
    assert(quietRead == 50);
    assert(buffer.framesInBuffer() == prevFrames);  // tail not moved

    // 10. Test zero-padding
    memset(samplesRead, 0, sizeof(samplesRead));
    CHECK_ERR(buffer.read(samplesRead, totalFrames, framesRead));
    assert(framesRead < totalFrames);
    for (size_t i = framesRead * channels; i < totalFrames * channels; ++i) {
        assert(samplesRead[i] == 0.0f);  // padded
    }

    LOGI("RingBuffer test passed.");
    return ERR_OK;
}
