#pragma once

#include <vector>

#include "asio_with_aliases.h"

namespace traft {

// TODO: won't work like this. Seems like it's needed to make one godlike class to manage raft chunk senders.
class GroupAwait {
public:
    GroupAwait(std::vector<asio::awaitable<void>> waiters, int32_t min_to_success)
        : waiters_(std::move(waiters)),
          min_to_success_(min_to_success) {
        // Nothing
    }

    asio::awaitable<void> wait(const asio::use_awaitable_t<>& token) {
        return async_initiate<const asio::use_awaitable_t<>, void (boost::system::error_code)>(
            [this](auto handler) {
                handler_.emplace(std::move(handler));
            }, token);
    }
private:
    using HandlerType = asio::async_result<asio::use_awaitable_t<>, void(boost::system::error_code)>::handler_type;
    std::optional<HandlerType> handler_;
    std::vector<asio::awaitable<void>> waiters_;
};

} // namespace traft