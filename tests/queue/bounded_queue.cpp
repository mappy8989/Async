#include <gtest/gtest.h>
#include <thread>

#include "queue/bounded_queue.hpp"

using namespace dispatcher::queue;

TEST(BoundQTest, push_pop_check) {
    BoundedQueue UQ(5);
    int check_value = 0;

    std::jthread push_thrd([&UQ, &check_value]() {
        for (int i = 0; i < 10; i++) {
            UQ.push([&i, &check_value]() { check_value++; });
        }
    });

    std::jthread pop_thrd([&UQ, &check_value]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::optional<std::function<void()>> func;
        for (int i = 1; i < 6; i++) {
            func = UQ.try_pop();
            EXPECT_TRUE(func.has_value());
            func.value()();
            EXPECT_EQ(i, check_value);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        for (int i = 6; i < 11; i++) {
            func = UQ.try_pop();
            EXPECT_TRUE(func.has_value());
            func.value()();
            EXPECT_EQ(i, check_value);
        }
    });
}