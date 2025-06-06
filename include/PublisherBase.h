#pragma once

#include <Topic.h>

template <typename T, typename E>
class PublisherBase {
public:
    using Callback = typename Topic<T, E>::Callback;

    void publish(const T& arg_a, const E& arg_b) {
        for (auto const &topic: _topics) {
            topic.publish(arg_a, arg_b);
        }
    }

    void subscribe(Topic<T, E> topic) {
        _topics.push_back(topic);
    }

    void unsubscribe(Topic<T, E> topic) {
        std::erase_if(_topics,
                      [&](const Callback &existing) {
                          return existing.target_type() == topic.target_type();
                      });
    }

protected:
    std::vector<Topic<T, E> > _topics;
};
