#include <gtest/gtest.h>

#include "condition_awaiter.h"

TEST(ConditionAwaiter, Defaults) {
    asio::io_context context(1);

    traft::ConditionAwaiter<int> awaiter(context.get_executor());

    bool finished = false;
    auto waiter = [&awaiter, &finished]() mutable -> asio::awaitable<void> {
        co_await awaiter.wait([](int v) { return v == 0; });
        finished = true;
    };

    boost::asio::co_spawn(context, waiter, boost::asio::detached);

    context.poll_one();
    EXPECT_FALSE(finished);

    context.poll_one();
    EXPECT_TRUE(finished);
}

TEST(ConditionAwaiter, Basics) {
    asio::io_context context(1);

    traft::ConditionAwaiter<int> awaiter(context.get_executor());

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

TEST(ConditionAwaiter, MultipleWait) {
    asio::io_context context(1);

    traft::ConditionAwaiter<int> awaiter(context.get_executor());

    bool finished1 = false;
    bool finished2 = false;
    auto waiter1 = [&awaiter, &finished1]() mutable -> asio::awaitable<void> {
        co_await awaiter.wait([](int v) { return v > 3; });
        finished1 = true;
    };
    auto waiter2 = [&awaiter, &finished2]() mutable -> asio::awaitable<void> {
        co_await awaiter.wait([](int v) { return v > 4; });
        finished2 = true;
    };

    boost::asio::co_spawn(context, waiter1, boost::asio::detached);
    boost::asio::co_spawn(context, waiter2, boost::asio::detached);

    context.poll();
    EXPECT_FALSE(finished1);
    EXPECT_FALSE(finished2);

    awaiter.updateData(4);
    context.poll();
    EXPECT_TRUE(finished1);
    EXPECT_FALSE(finished2);

    awaiter.updateData(5);
    context.poll();
    EXPECT_TRUE(finished1);
    EXPECT_TRUE(finished2);
}