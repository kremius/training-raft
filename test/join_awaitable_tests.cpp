#include <gtest/gtest.h>

#include <chrono>

#include "join_awaitable.h"

#include "logging.h"

using namespace std::chrono_literals;

TEST(FastestAwaitable, Basics) {
    asio::io_context context;

    struct Error {};
    struct Result {};
    struct Result2 { int v = 0; };

    auto generate = [&](auto duration, auto value) -> asio::awaitable<decltype(value)> {
        boost::asio::steady_timer timer(context, duration);
        co_await timer.async_wait(asio::use_awaitable);
        co_return value;
    };

    // TODO: check that timings are correct - the test should take ~6ms, not more or less
    auto coroutine = [&]() -> asio::awaitable<void> {
        EXPECT_TRUE(std::holds_alternative<Error>(
            co_await traft::fastest(generate(0ms, Error{}), generate(3ms, Result{}))));
        EXPECT_TRUE(std::holds_alternative<Error>(
            co_await traft::fastest(generate(3ms, Result{}), generate(0ms, Error{}))));
        EXPECT_TRUE(std::holds_alternative<Result>(
            co_await traft::fastest(generate(3ms, Error{}), generate(0ms, Result{}))));
        EXPECT_TRUE(std::holds_alternative<Result>(
            co_await traft::fastest(generate(0ms, Result{}), generate(3ms, Error{}))));

        EXPECT_TRUE(std::holds_alternative<Error>(
            co_await traft::fastest(generate(1ms, Error{}), generate(2ms, Result{}), generate(3ms, Result2{}))));
        EXPECT_TRUE(std::holds_alternative<Result>(
            co_await traft::fastest(generate(2ms, Error{}), generate(1ms, Result{}), generate(3ms, Result2{}))));
        EXPECT_TRUE(std::holds_alternative<Result2>(
            co_await traft::fastest(generate(3ms, Error{}), generate(2ms, Result{}), generate(1ms, Result2{}))));

        auto v1 = co_await traft::fastest(generate(0ms, Result2{2}), generate(1ms, Result2{1}));
        // TODO: ASSERT_EQ(v1.index(), 0);
        EXPECT_EQ(std::get<0>(v1).v, 2);

        auto v2 = co_await traft::fastest(generate(10ms, Result2{2}), generate(0ms, Result2{1}));
        // TODO: ASSERT_EQ(v1.index(), 0);
        EXPECT_EQ(std::get<1>(v2).v, 1);
    };

    boost::asio::co_spawn(context, coroutine, boost::asio::detached);
    context.run();
}