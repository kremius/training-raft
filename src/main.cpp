#include "listener.h"

#include <iostream>

#include <fmt/core.h>

int main(int /*argc*/, char */*argv*/[]) {
    try {
        boost::asio::io_context io_context(1);
        boost::asio::co_spawn(io_context, traft::listener, boost::asio::detached);
        io_context.run();
    } catch (const std::exception &ex) {
        fmt::print("Error: {}\n", ex.what());
    }
}
