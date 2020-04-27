#include "listener.h"

#include <fmt/core.h>

namespace traft
{

using tcp = boost::asio::ip::tcp;

asio::awaitable<void> listener() {
    auto executor = co_await asio::this_coro::executor;
    tcp::acceptor acceptor(executor, tcp::endpoint(tcp::v4(), 4321));

    fmt::print("Starting accept new connections...\n");

    while (true) {
        // TODO: errors handling (i.e. port already in use)
        tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);

        fmt::print("New incoming connection: {}\n", socket.remote_endpoint().address().to_string());

        socket.close();
    }
}

} // namespace traft