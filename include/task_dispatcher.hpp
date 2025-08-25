#pragma once

#include <memory>

#include "queue/priority_queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"

namespace dispatcher {

using namespace dispatcher::queue;
using namespace dispatcher::thread_pool;

class TaskDispatcher {
    // здесь ваш код
    QueueOptions high_prio_opt_;
    QueueOptions low_prio_opt_;

    size_t threads_num_;
    std::shared_ptr<PriorityQueue> prio_queue_;
    std::unique_ptr<ThreadPool> thrd_pool_;

public:
    TaskDispatcher(size_t thread_count = std::thread::hardware_concurrency(),
                   QueueOptions high_prio_opt = QueueOptions{true, 1000},
                   QueueOptions low_prio_opt = QueueOptions{false, std::nullopt});

    void schedule(TaskPriority priority, std::function<void()> task);
    ~TaskDispatcher() = default;
};

}  // namespace dispatcher