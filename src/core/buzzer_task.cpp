#include "buzzer_task.hpp"
#include <iostream>
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

BuzzerTask::BuzzerTask(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {
    if (logger_) logger_->info("BuzzerTask created");
}

BuzzerTask::~BuzzerTask() {
    if (logger_) logger_->info("BuzzerTask destroyed");
}

void BuzzerTask::run() {
    if (logger_) logger_->info("BuzzerTask running");
}

bool BuzzerTask::send_message(const IMessage& msg) {
    if (logger_) logger_->info("BuzzerTask send_message");
    return true;
}

} // namespace device_reminder
