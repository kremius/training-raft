#include <gtest/gtest.h>

#include <chrono>

#include "join_awaitable.h"

#include "logging.h"

using namespace std::chrono_literals;

TEST(AnyAwaitable, Basics) {
    asio::io_context context;

    struct Error {};
    struct Result {};

    auto generate = [&](auto duration, auto value) -> asio::awaitable<decltype(value)> {
        boost::asio::steady_timer timer(context, duration);
        co_await timer.async_wait(asio::use_awaitable);
        co_return value;
    };

    // TODO: check that timings are correct - the test should take ~6ms, not more or less
    auto coroutine = [&]() -> asio::awaitable<void> {
        EXPECT_TRUE(std::holds_alternative<Error>(
            co_await traft::any(generate(1ms, Error{}), generate(3ms, Result{}))));
        EXPECT_TRUE(std::holds_alternative<Error>(
            co_await traft::any(generate(3ms, Result{}), generate(1ms, Error{}))));
        EXPECT_TRUE(std::holds_alternative<Result>(
            co_await traft::any(generate(3ms, Error{}), generate(1ms, Result{}))));
        EXPECT_TRUE(std::holds_alternative<Result>(
            co_await traft::any(generate(1ms, Result{}), generate(3ms, Error{}))));
    };

    boost::asio::co_spawn(context, coroutine, boost::asio::detached);
    context.run();
}