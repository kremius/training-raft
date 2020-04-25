#include "test.h"

#include <iostream>
#include <chrono>

#include <boost/asio.hpp>

using namespace std::chrono_literals;

boost::asio::awaitable<void> foo() {
    boost::asio::executor io = co_await boost::asio::this_coro::executor;

    std::cout << "Hello" << std::endl;

    boost::asio::steady_timer timer(io, 1000ms);
    co_await timer.async_wait(boost::asio::use_awaitable);

    std::cout << "World" << std::endl;
}

void test() {
    boost::asio::io_context io_context(1);
    boost::asio::co_spawn(io_context, foo, boost::asio::detached);
    io_context.run();
}
