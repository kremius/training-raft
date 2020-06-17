#include "asio_with_aliases.h"
#include "logging.h"
#include "condition_awaiter.h"

using traft::operator""_format;
using namespace std::chrono_literals;

traft::ConditionAwaiter<int> *awaiter;

asio::awaitable<void> co_foo() {
    LOG(info) << "Awaiting for 10";
    co_await awaiter->wait([](auto value) { return value >= 10; }, asio::use_awaitable);
    LOG(info) << "Success!";
}

asio::awaitable<void> co_actions() {
    auto executor = co_await asio::this_coro::executor;
    while (true) {
        boost::asio::steady_timer timer(executor, 1000ms);
        co_await timer.async_wait(asio::use_awaitable);
        LOG(info) << "New data: {}"_format(awaiter->getData() + 1);
        awaiter->updateData(awaiter->getData() + 1);
    }
}

int main() {
    boost::asio::io_context io_context(1);

    traft::ConditionAwaiter<int> test(io_context.get_executor());

    test.updateData(0);
    awaiter = &test;

    boost::asio::co_spawn(io_context, co_foo, boost::asio::detached);
    boost::asio::co_spawn(io_context, co_actions, boost::asio::detached);

    io_context.run();

    return 0;
}
