#include "listener.h"

#include "logging.h"

namespace traft {

using tcp = boost::asio::ip::tcp;

asio::awaitable<void> listener() {
    try {
        auto executor = co_await asio::this_coro::executor;
        tcp::acceptor acceptor(executor, {tcp::v4(), 4321});

        LOG(info) << "Starting accept new connections on {}...\n"_format(acceptor.local_endpoint().port());

        while (true) {
            tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);

            LOG(info) << "New incoming connection: {}\n"_format(socket.remote_endpoint().address().to_string());

            socket.close();
        }
    } catch (const std::exception &ex) {
        LOG(error) << "Error: {}\n"_format(ex.what());
    }
}

} // namespace traft