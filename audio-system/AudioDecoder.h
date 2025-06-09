#pragma once

#include "IAudioSource.h"

struct ma_decoder;

/// @class AudioDecoder
/// @implements IAudioSource
class AudioDecoder final : public IAudioSource {
public:
    /// @brief Initialize the decoder object from the filename
    /// @param filename: path of audio file
    explicit AudioDecoder(const std::string& filename);
    ~AudioDecoder() override;

    /// @brief try to read
    /// @param out: Pointer to the output buffer
    /// @param frameCount: Number of frames to read
    /// @param framesRead: Number of frames actually read
    /// @returns ERR_OK if read was successful
    Err read(float* out, size_t frameCount, size_t &framesRead) override;
    bool jumpToTime(std::chrono::milliseconds time) override;
    bool jumpToFrame(size_t frame) override;
    void reset() override;

    size_t getTotalPcmFrames() const override;
    std::chrono::milliseconds getTotalDurationMs() const override;
    std::chrono::milliseconds getElapsedMs() const override;
    size_t getCurrentPcmFrame() const override;
    size_t getChannels() const override;
    size_t getSampleRate() const override;
    AudioSourceState getState() override;

private:
    /// @brief The path of the audio file
    std::string _filename;
    /// @brief The current state of the decoder.
    AudioSourceState _state;
    /// @brief Miniaudio Decoder struct.
    std::unique_ptr<ma_decoder> _decoder;
    /// @brief Total frames in the file.
    size_t _totalFrames;
};