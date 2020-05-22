#pragma once

#include <list>

#include "asio_with_aliases.h"

namespace traft {

// TODO: no synchronization currently. Is it needed really?
template<typename DataType>
class ConditionAwaiter {
    using HandlerType = asio::async_result<asio::use_awaitable_t<>, void(boost::system::error_code)>::handler_type;
    using ConditionType = std::function<bool(const DataType &)>;

public:
    ConditionAwaiter(asio::io_context *io_service) : io_service_(io_service) {
        // Nothing
    }

    asio::awaitable<void> wait(ConditionType condition, const asio::use_awaitable_t<>& token) {
        return async_initiate<const asio::use_awaitable_t<>, void (boost::system::error_code)>(
            [this](auto handler, ConditionType condition) {
                ConditionAndHandler waiter(std::move(condition), std::move(handler));
                if (tryWakeUp(&waiter)) {
                    return;
                }
                waiters_.emplace_back(std::move(waiter));
            }, token, std::move(condition));
    }

    const DataType &getData() const {
        return data_;
    }
    void updateData(DataType data) {
        data_ = std::move(data);
        for (auto it = waiters_.begin(); it != waiters_.end(); ++it) {
            if (!tryWakeUp(&(*it))) {
                continue;
            }
            it = waiters_.erase(it);
        }
    }

private:
    struct ConditionAndHandler {
        ConditionAndHandler(ConditionAndHandler&&) noexcept = default;
        ConditionAndHandler(ConditionType condition, HandlerType handler) noexcept
            : condition(std::move(condition)), handler(std::move(handler)) {
            // Nothing
        }

        ConditionType condition;
        HandlerType handler;
    };

    bool tryWakeUp(ConditionAndHandler *waiter) {
        if (!waiter->condition(data_)) {
            return false;
        }
        asio::post(*io_service_, [this, handler = std::move(waiter->handler)]() mutable {
            handler(boost::system::errc::make_error_code(boost::system::errc::success));
        });
        return true;
    }

    asio::io_context *io_service_;
    DataType data_;
    // It's std::list because std::vector::erase requires assignment operator,
    // and HandlerType lacks it.
    std::list<ConditionAndHandler> waiters_;
};

} // namespace traft