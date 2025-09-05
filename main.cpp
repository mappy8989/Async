#include <chrono>
#include <climits>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <print>
#include <thread>

#include "logger.hpp"
#include "task_dispatcher.hpp"

using namespace dispatcher;

int main() {
    /*    TaskDispatcher td((size_t)std::thread::hardware_concurrency());
        std::vector<std::jthread> threads;

        for (int i = 0; i < 5; ++i) {
            threads.emplace_back([&, i]() {
                for (int j = 0; j < 10; j++) {
                    td.schedule(TaskPriority::Normal,
                                [=]() { Logger::Get().Log("Normal priority message №" + std::to_string(10 * i + j)); });
                    td.schedule(TaskPriority::High,
                                [=]() { Logger::Get().Log("High priority message №" + std::to_string(10 * i + j)); });
                }
            });
        }
            */

    std::shared_ptr<dispatcher::queue::PriorityQueue> prio_queue =
        std::make_shared<PriorityQueue>(std::map<TaskPriority, QueueOptions>{
            {TaskPriority::High, QueueOptions(true, 100)}, {TaskPriority::Normal, QueueOptions(false, std::nullopt)}});
    ThreadPool tp(prio_queue, 4);
    for (int i = 0; i < 4; i++) {
        tp.push(TaskPriority::High, [&]() { std::println("thrd num = {}", i); });
    }

    std::println("Hello Async!");
}
