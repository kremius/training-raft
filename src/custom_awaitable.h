#include "asio_with_aliases.h"

#include <iostream>

class ManualBrake {
public:
    ManualBrake(asio::io_service& io_service) : io_service_(&io_service) {}

    asio::awaitable<int> pause(int value, const asio::use_awaitable_t<>& token) {
        return async_initiate<const asio::use_awaitable_t<>, void (boost::system::error_code, int)>(
            [this](auto handler, int v) {
                handler_.emplace(std::move(handler));
                value_ = v;
            }, token, value);
    }
    void unpause() {
        io_service_->post([this]() mutable {
            handler_.value()(boost::system::errc::make_error_code(boost::system::errc::success), value_ + 1);
            handler_.reset();
        });
    }
private:
    using HandlerType = asio::async_result<asio::use_awaitable_t<>, void(boost::system::error_code, int)>::handler_type;
    std::optional<HandlerType> handler_;
    int value_;
    asio::io_service *io_service_;
};