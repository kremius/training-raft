#include "test.h"

#include <iostream>

#include <boost/asio.hpp>

boost::asio::awaitable<void> foo() {
    std::cout << "Hello" << std::endl;
    std::cout << "World" << std::endl;
    co_return;
}

void test() {
    boost::asio::io_context io_context(1);
    boost::asio::co_spawn(io_context, foo, boost::asio::detached);
    io_context.run();
}
