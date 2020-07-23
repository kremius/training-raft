#pragma once

#include "asio_with_aliases.h"

namespace traft {

// This function executes the `callback` on the `to` executor and then
// awakes the coroutine with the result of the execution on the `from` executor.
// `from_executor` should be current executor.
// 'callback' should throw 'boost::system::system_error' exceptions
template<typename CallbackType>
asio::awaitable<std::invoke_result_t<CallbackType>> forward_call(
    asio::executor to_executor,
    CallbackType &&callback,
    const asio::use_awaitable_t<>& token = asio::use_awaitable) {
    auto from_executor = co_await asio::this_coro::executor;
    // Still somewhat unclear is guard really needed in coroutines, because
    // this_coro::executor somehow exists, so maybe some other work guard is
    // already created by co_spawn or something like that.
    auto guard = asio::make_work_guard(from_executor);
    // Using built-in asio free function in order to properly implement async call
    co_return co_await async_initiate<const asio::use_awaitable_t<>, void (error_code, std::invoke_result_t<CallbackType>)>(
        // The lambda right below is called after the coroutine suspension
        [from_executor, to_executor, guard = std::move(guard)](auto handler, CallbackType &&callback) {
            // Request execution of our 'callback' on the `to` executor
            asio::post(to_executor, [handler = std::move(handler), from_executor, callback = std::move(callback), guard = std::move(guard)]() mutable {
                try {
                    // Work's done on the line below
                    auto result = callback();
                    // Wake-up coroutine on the `from` executor
                    // TODO: maybe post here is not needed and everything is handled by handler?
                    asio::post(from_executor, [handler = std::move(handler), result = std::move(result)]() mutable {
                        // The line below will resume the coroutine which
                        // called 'co_await forward_call(to, [](){ /* something */ })' in the first place
                        handler(errc::make_error_code(errc::success), std::move(result));
                    });
                } catch (const boost::system::system_error &error) {
                    asio::post(from_executor, [handler = std::move(handler), code = error.code()]() mutable {
                        // The line below will resume the coroutine and throw exception
                        handler(code, std::invoke_result_t<CallbackType>{});
                    });
                }
            });
        }, token, std::forward<CallbackType>(callback));
}

} // namespace traft