#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "queue/priority_queue.hpp"
#include "types.hpp"

namespace dispatcher::thread_pool {

class ThreadPool {
public:
    explicit ThreadPool(std::shared_ptr<dispatcher::queue::PriorityQueue> prio_queue,
                        int threads_num = std::thread::hardware_concurrency());
    void push(TaskPriority priority, std::function<void()> task);
    ~ThreadPool();

private:
    void ThreadsStart(void);
    void Worker(void);

    std::atomic<bool> is_start_ = false;
    std::atomic<bool> is_active_ = true;

    std::vector<std::jthread> thrds_;
    size_t threads_num_;
    std::shared_ptr<dispatcher::queue::PriorityQueue> prio_queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

}  // namespace dispatcher::thread_pool
