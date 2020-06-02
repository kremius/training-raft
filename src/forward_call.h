#pragma once

#include "asio_with_aliases.h"

namespace traft {

// This function executes the `callback` on the `to` executor and then
// awakes the coroutine with the result of the execution on the `from` executor.
// `from_executor` should be current executor.
template<typename CallbackType>
asio::awaitable<std::invoke_result_t<CallbackType>> forward_call(
    asio::executor &from_executor,
    asio::executor &to_executor,
    CallbackType &&callback,
    const asio::use_awaitable_t<>& token = asio::use_awaitable) {
    // TODO: can we actually just get current executor with `co_await asio::this_coro::executor`
    // and then co_return async_initiate?
    // Using built-in asio free function in order to properly implement async call
    return async_initiate<const asio::use_awaitable_t<>, std::invoke_result_t<CallbackType> (boost::system::error_code)>(
        // The lambda right below is called after the coroutine suspension
        [](auto handler, asio::executor &from, asio::executor &to, CallbackType &&callback) {
            // Request execution of our 'callback' on the `to` executor
            asio::post(to, [handler = std::move(handler), from, callback = std::move(callback)]() mutable {
                // Work's done on the line below
                auto result = callback();
                // Wake-up coroutine on the `from` executor
                asio::post(from, [handler = std::move(handler), result = std::move(result)]() mutable {
                    // The line below will resume the coroutine which
                    // called 'co_await forward_call(from, to, [](){ /* something */ })' in the first place
                    hanlder(std::move(result));
                });
            });
        }, token, from_executor, to_executor, std::forward<CallbackType>(callback));
}

} // namespace traft