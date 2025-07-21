#pragma once

#include "infra/logger/i_logger.hpp"
#include <spdlog/logger.h>
#include <memory>

namespace device_reminder {

class Logger : public ILogger {
public:
    explicit Logger(std::shared_ptr<spdlog::logger> logger)
        : logger_(std::move(logger)) {}

    void info(const std::string& msg) override {
        logger_->info(msg);
    }

    void error(const std::string& msg) override {
        logger_->error(msg);
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace device_reminder
