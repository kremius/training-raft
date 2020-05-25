#include <gtest/gtest.h>

#include "condition_awaiter.h"

#include "logging.h"

TEST(ConditionAwaiter, Basics) {
    asio::io_context context(1);

    traft::ConditionAwaiter<int> awaiter(&context);

    bool finished = false;
    auto waiter = [&awaiter, &finished]() mutable -> asio::awaitable<void> {
        co_await awaiter.wait([](int v) { return v == 5; });
        finished = true;
    };

    boost::asio::co_spawn(context, waiter, boost::asio::detached);

    context.poll_one();
    EXPECT_FALSE(finished);
    awaiter.updateData(4);
    EXPECT_FALSE(finished);
    context.poll_one();
    EXPECT_FALSE(finished);
    awaiter.updateData(5);
    EXPECT_FALSE(finished);
    context.poll_one();
    EXPECT_TRUE(finished);
}