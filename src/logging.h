#pragma once

#include <boost/log/trivial.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#define LOG(level) BOOST_LOG_TRIVIAL(level)

namespace traft {

using namespace fmt::literals;

}