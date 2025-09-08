#include <gtest/gtest.h>
#include <print>

#include "queue/unbounded_queue.hpp"

using namespace dispatcher::queue;

TEST(UnBoundQTest, push_pop_check) {
    UnboundedQueue UQ;
    int check_value = 0;
    for (int i = 0; i < 5; i++) {
        UQ.push([&i, &check_value]() { check_value++; });
    }

    std::optional<std::function<void()>> func;
    // starting from 1 because check_value increments before EXPECT_EQ
    for (int i = 1; i < 6; i++) {
        func = UQ.try_pop();
        EXPECT_TRUE(func.has_value());
        func.value()();
        EXPECT_EQ(i, check_value);
    }

    func = UQ.try_pop();
    EXPECT_EQ(func, std::nullopt);
}

// здесь ваш код