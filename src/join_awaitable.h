#pragma once

#include <variant>

#include "asio_with_aliases.h"

namespace traft {

template<typename LeftType, typename RightType>
asio::awaitable<std::variant<LeftType, RightType>> any(
    asio::awaitable<LeftType> left,
    asio::awaitable<RightType> right,
    const asio::use_awaitable_t<>& token = asio::use_awaitable) {
    auto executor = co_await asio::this_coro::executor;
    // TODO: executor should always be strand

    using HandlerFunction = void(error_code, std::variant<LeftType, RightType>);
    using HandlerType = typename asio::async_result<asio::use_awaitable_t<>, HandlerFunction>::handler_type;

    auto handler_store = std::make_shared<std::optional<HandlerType>>();

    // Using built-in asio free function in order to properly implement async call
    co_return co_await async_initiate<const asio::use_awaitable_t<>, HandlerFunction>(
        // The lambda right below is called after the coroutine suspension
        [&executor, left = std::move(left), right = std::move(right), handler_store = std::move(handler_store)](auto handler) mutable {
            handler_store->emplace(std::move(handler));
            boost::asio::co_spawn(
                executor,
                // TODO: check that 'executor' and 'left' won't be removed too early
                [&executor, left = std::move(left), handler_store]() mutable -> asio::awaitable<void> {
                    auto result = co_await std::move(left);
                    if (!handler_store->has_value()) {
                        // We're too late - other awaiter was faster.
                        co_return;
                    }
                    asio::post(executor, [result = std::move(result), handler = std::move(handler_store->value())]() mutable {
                        // TODO: exceptions?
                        handler(errc::make_error_code(errc::success), std::move(result));
                    });
                    handler_store->reset();
                }, boost::asio::detached);
            boost::asio::co_spawn(
                executor,
                // TODO: check that 'executor' and 'left' won't be removed too early
                [&executor, right = std::move(right), handler_store]() mutable -> asio::awaitable<void> {
                    auto result = co_await std::move(right);
                    if (!handler_store->has_value()) {
                        // We're too late - other awaiter was faster.
                        co_return;
                    }
                    asio::post(executor, [result = std::move(result), handler = std::move(handler_store->value())]() mutable {
                        // TODO: exceptions?
                        handler(errc::make_error_code(errc::success), std::move(result));
                    });
                    handler_store->reset();
                }, boost::asio::detached);
        }, token);
}

} // namespace traft