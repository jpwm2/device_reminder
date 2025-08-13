#include "infra/logger.hpp"

namespace device_reminder {

Logger::Logger(std::shared_ptr<spdlog::logger> logger)
    : logger_(std::move(logger)) {}

void Logger::info(const std::string& msg) {
    logger_->info(msg);
}

void Logger::error(const std::string& msg) {
    logger_->error(msg);
}

void Logger::warn(const std::string& msg) {
    logger_->warn(msg);
}

} // namespace device_reminder
