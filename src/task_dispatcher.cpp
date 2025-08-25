#include "task_dispatcher.hpp"

namespace dispatcher {

TaskDispatcher::TaskDispatcher(size_t thread_count, QueueOptions high_prio_opt,
                               QueueOptions low_prio_opt)
    : threads_num_(thread_count), high_prio_opt_(high_prio_opt), low_prio_opt_(low_prio_opt) {
    prio_queue_ = std::make_shared<PriorityQueue>(std::map<TaskPriority, QueueOptions>{
        {TaskPriority::High, high_prio_opt_}, {TaskPriority::Normal, low_prio_opt_}});

    thrd_pool_ = std::make_unique<ThreadPool>(prio_queue_, threads_num_);
}

void TaskDispatcher::schedule(TaskPriority priority, std::function<void()> task) {
    prio_queue_->push(priority, task);
}

}  // namespace dispatcher