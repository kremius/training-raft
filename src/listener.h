#pragma once

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace traft
{

asio::awaitable<void> listener();

} // namespace traft
