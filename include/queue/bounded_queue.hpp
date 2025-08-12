#pragma once
#include "queue/queue.hpp"
#include <condition_variable>
#include <deque>
#include <mutex>

namespace dispatcher::queue {

class BoundedQueue : public IQueue {
    size_t capacity_;
    std::deque<std::function<void()>> queue_;

    std::mutex mutex_;
    std::condition_variable not_full_;
    // здесь ваш код
public:
    explicit BoundedQueue(int capacity) : capacity_(capacity) {}

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~BoundedQueue() override;
};

}  // namespace dispatcher::queue