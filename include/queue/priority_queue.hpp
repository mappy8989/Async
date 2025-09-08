#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <condition_variable>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace dispatcher::queue {

class PriorityQueue {
    // здесь ваш код
    std::map<TaskPriority, std::unique_ptr<IQueue>> map_;

    std::mutex mutex_;
    std::condition_variable cond_;

    bool is_active_ = false;

public:
    explicit PriorityQueue(std::map<TaskPriority, QueueOptions> &&prio_map);

    void push(TaskPriority priority, std::function<void()> task);
    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<std::function<void()>> pop();

    void shutdown();

    ~PriorityQueue() {}
};

}  // namespace dispatcher::queue