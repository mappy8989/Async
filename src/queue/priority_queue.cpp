#include "queue/priority_queue.hpp"
#include "queue/bounded_queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

namespace dispatcher::queue {

PriorityQueue::PriorityQueue(std::map<TaskPriority, QueueOptions> &prio_map) {
    for (auto &elem : prio_map) {
        if (elem.second.bounded) {
            map_.emplace(elem.first, std::make_unique<dispatcher::queue::BoundedQueue>(elem.second.capacity.value()));
        } else {
            map_.emplace(elem.first, std::make_unique<dispatcher::queue::UnboundedQueue>(elem.second.capacity.value()));
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

    if (auto it = map_.find(TaskPriority::High); it != map_.end()) {
        return it->second->try_pop();
    } else if (auto it = map_.find(TaskPriority::Normal); it != map_.end()) {
        return it->second->try_pop();
    }
}

void shutdown() {}

}  // namespace dispatcher::queue