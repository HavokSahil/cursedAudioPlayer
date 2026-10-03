#pragma once

#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include "err_codes.h"

template<typename T, typename E>
class Topic {
public:
    using Callback = std::function<void(T, E)>;

    explicit Topic(const char *id): _id(id) {}

    using Subscription = size_t;

    Subscription subscribe(const Callback &cb) {
        const Subscription id = _nextSubscription++;
        _subscribers.emplace_back(id, cb);
        return id;
    }

    void unsubscribe(const Subscription id) {
        std::erase_if(_subscribers, [id](const auto& existing) {
            return existing.first == id;
        });
    }

    void publish(const T& event_arg_a, const E& event_arg_b) {
        for (auto const &[id, callback]: _subscribers) {
            callback(event_arg_a, event_arg_b);
        }
    }

    bool operator==(const Topic &other) const {
        return _id == other._id;
    }

private:
    std::string _id;
    Subscription _nextSubscription{0};
    std::vector<std::pair<Subscription, Callback>> _subscribers;
};
