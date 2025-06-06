#pragma once
#include <chrono>
#include <memory>
#include <thread>

#include "PublisherBase.h"

class IAudioSink;
class IAudioSource;
class RingBuffer;

typedef struct audio_system_info {
    const char* name;
    const char* description;
    const char* format;
    size_t sample_rate;
    size_t channels;
    size_t total_frames;
    double bitrate;                         // in Kbps
    std::chrono::milliseconds durationMs;
} AudioSystemInfo;

typedef struct audio_system_state {
    bool isPlaying;
    bool isMute;
    float volume;
    bool isLoop;
    float speed;
    size_t currentFrame;
    std::chrono::milliseconds elapsedMs;
} AudioSystemState;

using AudioTopic = Topic<float*, size_t>;
using AudioPublisher = PublisherBase<float*, size_t>;

class AudioSystem final: public AudioPublisher {
public:
    explicit AudioSystem(std::string &filename, size_t bufferSize);
    ~AudioSystem();
    size_t getCurrentFrame() const;
    size_t getTotalFrames() const;
    std::chrono::milliseconds getElapsedTimeMs() const;
    std::chrono::milliseconds getTotalDurationMs() const;
    AudioSystemInfo getInfo() const;
    AudioSystemState getState() const;

    bool getIsPlaying() const;
    bool getIsMute() const;
    float getVolume() const;
    float getSpeed() const;

    void setIsPlaying(bool isPlaying);
    void setVolume(float volume);
    void setSpeed(float speed);
    void setIsMute(bool isMute);

    void seek(std::chrono::milliseconds time);
    void seek(size_t frame);
    void reset();

private:
    std::thread _thread;

    std::unique_ptr<IAudioSource> _audioSource;
    std::unique_ptr<RingBuffer> _ringBuffer;
    std::unique_ptr<IAudioSink> _audioSink;

    std::atomic<bool> _isPlaying{false};
    bool _isMute {false};
    float _volume {1.0};
    bool _isLoop {false};
    float _speed {1.0};
    size_t chunkSize{1024};

    void _background_loop();
};
