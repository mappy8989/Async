#include "queue/unbounded_queue.hpp"

#include <functional>
#include <mutex>
#include <queue>
#include <semaphore>

namespace dispatcher::queue {

void UnboundedQueue::push(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(mutex_);

    queue_.push_back(std::move(task));
}

std::optional<std::function<void()>> UnboundedQueue::try_pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    if (queue_.empty()) {
        return std::nullopt;
    }

    std::function func = std::move(queue_.front());

    queue_.pop_front();

    return func;
}

}  // namespace dispatcher::queue