#include "listener.h"

#include <fmt/core.h>

namespace traft
{

using tcp = boost::asio::ip::tcp;

asio::awaitable<void> listener() {
    try {
        auto executor = co_await asio::this_coro::executor;
        tcp::acceptor acceptor(executor, {tcp::v4(), 4321});

        fmt::print("Starting accept new connections...\n");

        while (true) {
            tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);

            fmt::print("New incoming connection: {}\n", socket.remote_endpoint().address().to_string());

            socket.close();
        }
    } catch (const std::exception &ex) {
        fmt::print("Error: {}\n", ex.what());
    }
}

} // namespace traft