#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

#include "queue/priority_queue.hpp"
#include "types.hpp"

using namespace dispatcher;
using namespace dispatcher::queue;

TEST(PrioQTest, allOfCheck) {
    int high_prio_val = 0;
    int normal_prio_val = 0;

    constexpr size_t PRIO_Q_SIZE = 5;
    PriorityQueue PQ(
        std::move(std::map<TaskPriority, QueueOptions>{{TaskPriority::High, QueueOptions{true, PRIO_Q_SIZE}},
                                                       {TaskPriority::Normal, QueueOptions{false, std::nullopt}}}));

    bool is_ready = false;
    std::condition_variable cv;
    std::mutex mtx;

    std::jthread high_q_thrd([&]() {
        std::unique_lock lock(mtx);
        for (int i = 0; i < PRIO_Q_SIZE; i++) {
            PQ.push(TaskPriority::Normal, [&]() { normal_prio_val++; });
        }

        for (int i = 0; i < PRIO_Q_SIZE; i++) {
            PQ.push(TaskPriority::High, [&]() { high_prio_val++; });
        }

        is_ready = true;
        cv.notify_one();
    });

    std::jthread high_q_thrd_pop([&]() {
        std::unique_lock lock(mtx);
        cv.wait(lock, [&]() {
            if (is_ready)
                return true;
            return false;
        });

        for (int i = 0; i < PRIO_Q_SIZE; i++) {
            auto func = PQ.pop();

            if (func.has_value()) {
                func.value()();
            }
        }
        EXPECT_EQ(high_prio_val, 5);
        EXPECT_EQ(normal_prio_val, 0);

        for (int i = 0; i < PRIO_Q_SIZE; i++) {
            auto func = PQ.pop();

            if (func.has_value()) {
                func.value()();
            }
        }
        EXPECT_EQ(normal_prio_val, 5);
    });
}

// здесь ваш код