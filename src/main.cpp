// #include "listener.h"
// #include "logging.h"

// using traft::operator""_format;
// using namespace std::chrono_literals;

// #include "custom_awaitable.h"

#include <iostream>

#include <boost/asio.hpp>

namespace asio = boost::asio;
namespace sys = boost::system;

using namespace std::chrono_literals;

using TokenType = asio::use_awaitable_t<>;
using HandlerType = asio::async_result<TokenType, void(sys::error_code, int)>::handler_type;

// TODO: proper synchronization.
class ManualBrake {
public:
    ManualBrake(asio::io_service& io_service) : io_service_(&io_service) {}

    asio::awaitable<int> pause(int value, const TokenType& token) {
        return async_initiate<const TokenType, void (sys::error_code, int)>(
            [this](HandlerType handler, int value) {
                handler_.emplace(std::move(handler));
                value_ = value;
            }, token, value);
    }

    void unpause(int supplement) {
        io_service_->post([this, supplement] {
            (*handler_)(sys::errc::make_error_code(sys::errc::success), value_ + supplement);
        });
    }

private:
    std::optional<HandlerType> handler_;
    int value_;
    asio::io_service *io_service_;
};

ManualBrake *brake;

asio::awaitable<void> co_foo() {
    std::cout << "Before pause" << std::endl;
    const int result = co_await brake->pause(42, asio::use_awaitable);
    std::cout << "After pause " << result << std::endl;
}

int main() {
    boost::asio::io_context io_context(1);

    ManualBrake brake_local(io_context);
    brake = &brake_local;

    boost::asio::co_spawn(io_context, co_foo, boost::asio::detached);

    boost::asio::steady_timer timer(io_context, 10000ms);
    timer.async_wait([&io_context] (sys::error_code) {
        // Do something meaningful and then wake up coroutine
        brake->unpause(24);
    });

    io_context.run();

    return 0;
}
