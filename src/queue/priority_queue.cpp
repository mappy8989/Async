#include "queue/priority_queue.hpp"
#include "queue/bounded_queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

namespace dispatcher::queue {

PriorityQueue::PriorityQueue(std::map<TaskPriority, QueueOptions> &&prio_map) {
    for (auto &&elem : prio_map) {
        if (elem.second.bounded) {
            map_.emplace(elem.first, std::make_unique<dispatcher::queue::BoundedQueue>(
                                         elem.second.capacity.value_or(0)));
        } else {
            map_.emplace(elem.first, std::make_unique<dispatcher::queue::UnboundedQueue>(
                                         elem.second.capacity.value_or(0)));
        }
    }
}

void PriorityQueue::push(TaskPriority priority, std::function<void()> task) {
    if (auto it = map_.find(priority); it != map_.end()) {
        it->second->push(task);
    }
}

std::optional<std::function<void()>> PriorityQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);

    auto check_queues = [this]() -> std::pair<bool, std::optional<std::function<void()>>> {
        if (auto it = map_.find(TaskPriority::High); it != map_.end()) {
            return std::make_pair(true, it->second->try_pop());
        } else if (auto it = map_.find(TaskPriority::Normal); it != map_.end()) {
            return std::make_pair(true, it->second->try_pop());
        }

        return std::make_pair(false, std::nullopt);
    };

    std::function<void()> func;

    cond_.wait(lock, [this, &check_queues, &func] {
        auto get_from_queue = check_queues();
        if ((!get_from_queue.first || !get_from_queue.second.has_value()) && is_active_) {
            return false;
        } else if (get_from_queue.first && get_from_queue.second.has_value()) {
            func = std::move(get_from_queue.second.value());
            return true;
        } else if (!is_active_) {
            return true;
        }

        return false;
    });

    if (!is_active_) {
        return std::nullopt;
    }

    return func;
}

void PriorityQueue::shutdown() {
    std::lock_guard lock(mutex_);
    is_active_ = false;

    cond_.notify_one();
}

}  // namespace dispatcher::queue