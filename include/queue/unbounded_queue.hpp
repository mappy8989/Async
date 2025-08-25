#pragma once
#include "queue/queue.hpp"
#include <deque>
#include <mutex>

namespace dispatcher::queue {

class UnboundedQueue : public IQueue {
    std::deque<std::function<void()>> queue_;

    std::mutex mutex_;
    // здесь ваш код
public:
    explicit UnboundedQueue(int capacity = 0) {}

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~UnboundedQueue() override {};
};

}  // namespace dispatcher::queue