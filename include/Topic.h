#pragma once

#include <functional>
#include "err_codes.h"

template<typename T, typename E>
class Topic {
public:
    using Callback = std::function<Err(T, E)>;

    void subscribe(const Callback &cb) {
        _subscribers.push_back(cb);
    }

    void unsubscribe(const Callback &cb) {
        std::erase_if(_subscribers,
                      [&](const Callback &existing) {
                          return existing.target_type() == cb.target_type();
                      });
    }

    void publish(const T& event_arg_a, const E &event_arg_b) {
        for (auto const &subs: _subscribers) {
            subs(event_arg_a, event_arg_b);
        }
    }

private:
    std::vector<Callback> _subscribers;
};
