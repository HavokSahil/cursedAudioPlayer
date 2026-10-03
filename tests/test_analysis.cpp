#include <cassert>
#include <cmath>
#include <complex>
#include <numbers>
#include <vector>

#include "AudioQueue.h"
#include "features/FFT.h"

int test_analysis() {
    AudioQueue<float> queue(3, 2);
    AudioQueue<float> other(2, 3);
    float first[] = {2, 4, 6};
    float second[] = {8, 10, 12};
    queue.pushBuffer(first, 3);
    other.pushBuffer(first, 3);
    queue.pushBuffer(second, 3);
    float values[3];
    queue.fullPeek(values);
    assert(values[0] == 3 && values[1] == 7 && values[2] == 11);
    assert(other.peek().back() == 4);

    queue.reset();
    queue.fullPeek(values);
    assert(values[0] == 0 && values[1] == 0 && values[2] == 0);

    bool rejected = false;
    try { AudioQueue<float> invalid(2, 0); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);

    Topic<float, size_t> events("events");
    PublisherBase<float, size_t> publisher;
    publisher.subscribe(events);
    float received = 0;
    auto makeCallback = [&](float scale) {
        return [&, scale](float value, size_t) { received += value * scale; };
    };
    const auto subscription = events.subscribe(makeCallback(1));
    events.subscribe(makeCallback(2));
    events.unsubscribe(subscription);
    publisher.publish(3.0f, 1);
    assert(received == 6);  // Unsubscribe only the requested callback of the same type.
    publisher.unsubscribe(events);
    publisher.publish(3.0f, 1);
    assert(received == 6);

    constexpr size_t size = 32;
    FFT<float> fft(size);
    std::vector<std::complex<float>> result(size);
    Topic<std::complex<float>*, size_t> topic("test");
    topic.subscribe([&](std::complex<float>* data, size_t count) {
        std::copy_n(data, count, result.begin());
    });
    fft.subscribe(topic);
    std::vector<float> input(size);
    for (size_t i = 0; i < size; ++i)
        input[i] = std::cos(2 * std::numbers::pi * 3 * i / size);
    fft.fill(input.data(), size);
    fft.compute();
    fft.publishResult();
    for (size_t i = 0; i < size; ++i) {
        const float expected = (i == 3 || i == size - 3) ? size / 2.0f : 0.0f;
        assert(std::abs(std::abs(result[i]) - expected) < 0.001f);
    }
    fft.compute(true);
    fft.publishResult();
    for (size_t i = 0; i < size; ++i)
        assert(std::abs(result[i] - std::complex<float>(input[i], 0)) < 0.001f);

    rejected = false;
    try { FFT<float> invalid(3); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    return 0;
}
