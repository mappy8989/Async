#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

#include "queue/bounded_queue.hpp"

using namespace dispatcher::queue;

TEST(BoundQTest, push_pop_check) {
    std::mutex mutex_;

    BoundedQueue UQ(5);
    int check_value = 0;
    std::condition_variable cond_;
    bool ready_to_pop = false;
    bool ready_to_push = false;

    std::jthread push_thrd([&]() {
        std::unique_lock<std::mutex> lock(mutex_);
        for (int i = 0; i < 10; i++) {
            if (i == 5) {
                ready_to_pop = true;
                // unblock pop_thrd
                cond_.notify_one();
                cond_.wait(lock, [&]() {
                    if (ready_to_push)
                        return true;
                    return false;
                });
            }
            UQ.push([&]() { check_value++; });
        }
        // unblock pop_thrd to get the remaining tasks
        ready_to_pop = true;
        cond_.notify_one();
    });

    std::jthread pop_thrd([&]() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [&]() {
            if (ready_to_pop) {
                return true;
            };
            return false;
        });

        ready_to_pop = false;
        std::optional<std::function<void()>> func;
        for (int i = 1; i < 6; i++) {
            func = UQ.try_pop();
            EXPECT_TRUE(func.has_value());
            func.value()();
            EXPECT_EQ(i, check_value);
        }
        ready_to_push = true;
        cond_.notify_one();

        cond_.wait(lock, [&]() {
            if (ready_to_pop) {
                return true;
            };
            return false;
        });
        for (int i = 6; i < 11; i++) {
            func = UQ.try_pop();
            EXPECT_TRUE(func.has_value());
            func.value()();
            EXPECT_EQ(i, check_value);
        }
    });
}