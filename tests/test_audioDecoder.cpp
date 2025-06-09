#include "AudioDecoder.h"
#include <cassert>
#include <iostream>

#ifndef TEST_MEDIA_DIR
    #error "Test media directory not specified"
#endif

#include "err_codes.h"

int test_audio_decoder() {
    const std::string filename = std::format("{}/imperial_march.wav", TEST_MEDIA_DIR);;
    AudioDecoder decoder(filename);
    assert(decoder.getState() != AudioSourceState::FINISHED);

    const size_t channels = decoder.getChannels();
    const size_t sampleRate = decoder.getSampleRate();
    const size_t totalFrames = decoder.getTotalPcmFrames();

    std::cout << "Channels: " << channels << "\n";
    std::cout << "Sample Rate: " << sampleRate << "\n";
    std::cout << "Total Frames: " << totalFrames << "\n";

    const size_t bufferSize = channels * 512 * sizeof(float);
    std::cout << "Buffer Size: " << bufferSize << "\n";
    fflush(stdout);
    const auto buffer = std::make_unique<float[]>(bufferSize);

    // 1. Read some frames
    size_t framesRead;
    assert(decoder.read(buffer.get(), 512, framesRead) == ERR_OK);
    assert(framesRead > 0);
    std::cout << "Read " << framesRead << " frames\n";

    // 2. Jump to middle of audio
    assert(decoder.jumpToFrame(totalFrames / 2));
    assert(decoder.getCurrentPcmFrame() >= totalFrames / 2);

    // 3. Read again
    assert(decoder.read(buffer.get(), 256, framesRead) == ERR_OK);
    assert(framesRead > 0);

    // 4. Jump to time-based position (~5 seconds)
    const std::chrono::milliseconds jumpTime(5000);
    assert(decoder.jumpToTime(jumpTime));
    const auto actualTime = decoder.getElapsedMs();
    std::cout << "Jumped to time: " << actualTime.count() << " ms\n";
    assert(std::abs((int64_t)actualTime.count() - 5000) < 100); // within 0.1s

    // 5. Reset decoder
    decoder.reset();
    assert(decoder.getCurrentPcmFrame() == 0);
    assert(decoder.getState() == AudioSourceState::READY);

    // 6. Read until EOF
    size_t totalRead = 0;
    while (decoder.getState() != AudioSourceState::FINISHED) {
        size_t r;
        assert(decoder.read(buffer.get(), 1024, r) == ERR_OK);
        if (r == 0) break;
        totalRead += r;
    }

    std::cout << "Total frames read until EOF: " << totalRead << "\n";
    assert(totalRead <= totalFrames);

    // 7. Read after EOF — should return 0
    assert(decoder.read(buffer.get(), 128, framesRead) == ERR_OK);
    assert(framesRead == 0);

    LOGI("AudioDecoder test passed.");
    return ERR_OK;
}
