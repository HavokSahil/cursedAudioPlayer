#include "AudioPlayer.h"
#include "RingBuffer.h"
#include <iostream>
#include <cassert>
#include <vector>

int test_audio_player() {
    constexpr size_t sampleRate = 44100;
    constexpr size_t channels = 2;
    constexpr size_t totalFrames = 2048;

    // 1. Setup test ring buffer
    RingBuffer ringBuffer(channels, totalFrames);

    // Fill buffer with dummy data
    std::vector<float> dummyData(channels * totalFrames, 0.25f);
    size_t written = 0;
    Err err = ringBuffer.write(dummyData.data(), totalFrames, written);
    assert(err == ERR_OK);
    assert(written == totalFrames);

    // 2. Define data callback: read from ring buffer
    AudioPlayer::DataCallback dataCb = [&](float* out, size_t frameCount, size_t& framesRead) -> Err {
        return ringBuffer.read(out, frameCount, framesRead);
    };

    // 3. Define query callback: check frames available
    AudioPlayer::QueryCallback queryCb = [&]() -> size_t {
        return ringBuffer.framesInBuffer();
    };

    // 4. Construct player
    AudioPlayer player(sampleRate, channels, dataCb, queryCb);

    // 5. Check initial state
    assert(player.getState() == AudioSinkState::PAUSED);

    // 6. Test play transition
    Err playErr = player.play();
    assert(playErr == ERR_OK);
    assert(player.getState() == AudioSinkState::PLAYING);

    // 7. Test pause transition
    Err pauseErr = player.pause();
    assert(pauseErr == ERR_OK);
    assert(player.getState() == AudioSinkState::PAUSED);

    // 8. Test redundant transitions
    pauseErr = player.pause();
    assert(pauseErr != ERR_OK);
    assert(player.getState() == AudioSinkState::PAUSED);

    // 9. Reset and play again
    assert(player.play() == ERR_OK);
    assert(player.getState() == AudioSinkState::PLAYING);

    // 10. Test reading after exhaustion
    float tempBuf[1024 * channels];
    size_t framesRead = 0;
    err = dataCb(tempBuf, 1024, framesRead);
    assert(err == ERR_OK);
    std::cout << "Frames read from buffer: " << framesRead << "\n";
    assert(framesRead <= 1024);

    std::cout << "AudioPlayer test passed.\n";
    return ERR_OK;
}
