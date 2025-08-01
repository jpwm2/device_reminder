#pragma once

#include "infra/logger/logger.hpp"
#include "infra/logger/i_logger.hpp"

#include <boost/di.hpp>
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace di = boost::di;
namespace device_reminder {

inline auto make_logger_injector() {
    auto spd_logger = std::make_shared<spdlog::logger>(
        "",
        std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt")
    );

    // バインディングの束を返す（injectorは返さない）
    return std::tuple{
        di::bind<ILogger>.to<Logger>(),
        di::bind<spdlog::logger>.to(spd_logger).in(di::singleton)
    };
}

} // namespace device_reminder
