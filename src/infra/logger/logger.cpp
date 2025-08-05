#include "infra/logger/logger.hpp"
#include <utility>

namespace device_reminder {

Logger::Logger(std::shared_ptr<spdlog::logger> logger)
    : logger_(std::move(logger)) {}

void Logger::info(const std::string& msg) {
    // spdlog ロガーを用いて情報レベルのメッセージを出力する
    logger_->info(msg);
}

void Logger::warn(const std::string& msg) {
    // spdlog ロガーを用いて警告レベルのメッセージを出力する
    logger_->warn(msg);
}

void Logger::error(const std::string& msg) {
    // spdlog ロガーを用いてエラーレベルのメッセージを出力する
    logger_->error(msg);
}

} // namespace device_reminder
