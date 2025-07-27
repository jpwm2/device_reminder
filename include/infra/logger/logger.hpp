#pragma once

#include "infra/logger/i_logger.hpp"
#include <spdlog/logger.h>
#include <memory>

namespace device_reminder {

class Logger : public ILogger {
public:
    explicit Logger(std::shared_ptr<spdlog::logger> logger);

    void info(const std::string& msg) override;
    void error(const std::string& msg) override;
    void warn(const std::string& msg) override;

private:
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace device_reminder
