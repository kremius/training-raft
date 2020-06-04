#pragma once

#define BOOST_ASIO_HAS_MOVE 1

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace traft {

using error_code = boost::system::error_code;
namespace errc = boost::system::errc;

} // namespace traft