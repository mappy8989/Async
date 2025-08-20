#include "thread_pool/thread_pool.hpp"

namespace dispatcher::thread_pool {

ThreadPool::ThreadPool(std::shared_ptr<dispatcher::queue::PriorityQueue> prio_queue, int threads_num)
    : prio_queue_(std::move(prio_queue)), threads_num_(threads_num) {
    thrds_.reserve(threads_num);
}

void ThreadPool::push(TaskPriority priority, std::function<void()> task) {
    prio_queue_.get()->push(priority, std::move(task));
}

ThreadPool::~ThreadPool() {
    is_active_ = false;
    prio_queue_->shutdown();

    for (auto &worker : thrds_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::ThreadsStart(void) {
    for (int i = 0; i < threads_num_; i++) {
        thrds_.emplace_back(&ThreadPool::Worker, this);
    }
}

void ThreadPool::Worker(void) {
    std::unique_lock<std::mutex> lock(mutex_);
    while (is_active_) {
        std::optional<std::function<void()>> task = prio_queue_->pop();

        if (task.has_value()) {
            task.value();
        } else {
            std::this_thread::yield();
        }
    }
}

}  // namespace dispatcher::thread_pool