#include "listener.h"

int main(int /*argc*/, char */*argv*/[]) {
    boost::asio::io_context io_context(1);
    boost::asio::co_spawn(io_context, traft::listener, boost::asio::detached);
    io_context.run();
}
