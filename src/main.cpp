#include "listener.h"
#include "logging.h"

using traft::operator""_format;
using namespace std::chrono_literals;

#include "custom_awaitable.h"

ManualBrake *brake;

asio::awaitable<void> co_foo() {
    std::cout << "Before pause" << std::endl;
    const int result = co_await brake->pause(15, asio::use_awaitable);
    std::cout << "After pause " << result << std::endl;
}

int main(int /*argc*/, char */*argv*/[]) {
    // try {
    //     boost::asio::io_context io_context(1);
    //     boost::asio::co_spawn(io_context, traft::listener, boost::asio::detached);
    //     io_context.run();
    // } catch (const std::exception &ex) {
    //     LOG(error) << "Error: {}"_format(ex.what());
    // }
    boost::asio::io_context io_context(1);

    ManualBrake brake_local(io_context);
    brake = &brake_local;

    boost::asio::co_spawn(io_context, co_foo, boost::asio::detached);

    boost::asio::steady_timer timer(io_context, 2000ms);
    timer.async_wait([&io_context] (boost::system::error_code) {
        // Do something meaningful and then wake up coroutine
        io_context.post([] { brake->unpause(); });
    });

    io_context.run();
}
