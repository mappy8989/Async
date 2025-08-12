#include "queue/bounded_queue.hpp"

namespace dispatcher::queue {

void BoundedQueue::push(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(mutex_);

    not_full_.wait(lock, [this] { return capacity_ > queue_.size(); });

    queue_.push_back(std::move(task));
}

std::optional<std::function<void()>> BoundedQueue::try_pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    if (queue_.empty()) {
        return std::nullopt;
    }

    std::function func = std::move(queue_.front());

    queue_.pop_front();

    return func;
}

}  // namespace dispatcher::queue