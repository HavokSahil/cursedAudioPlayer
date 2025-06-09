#pragma once

#include <Topic.h>

template <typename T, typename E>
class PublisherBase {
public:
    virtual ~PublisherBase() = default;

    using Callback = typename Topic<T, E>::Callback;

    virtual void publish(const T& arg_a, const E& arg_b) {
        for (auto &topic: _topics) {
            topic.publish(arg_a, arg_b);
        }
    }

    virtual void publish(const T& arg_a, const E& arg_b, const size_t topic_idx) {
        if (_topics.size() > topic_idx) {
            _topics[topic_idx].publish(arg_a, arg_b);
        }
    }

    virtual void subscribe(Topic<T, E> &topic) {
        _topics.push_back(topic);
    }

    virtual void unsubscribe(Topic<T, E> &topic) {
        std::erase_if(_topics,
                      [&](Topic<T, E> &existing) {
                          return existing == topic;
                      });
    }

protected:
    std::vector<Topic<T, E>>_topics;
};
