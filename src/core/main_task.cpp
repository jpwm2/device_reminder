#include "main_task.hpp"
#include "message/i_message.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

MainTask::MainTask(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {
    if (logger_) logger_->info("MainTask created");
}

MainTask::~MainTask() {
    if (logger_) logger_->info("MainTask destroyed");
}

bool MainTask::send_message(const IMessage& msg) {
    if (logger_) logger_->info("MainTask send_message");
    return true;
}

void MainTask::run() {
    if (logger_) logger_->info("MainTask running");
}

} // namespace device_reminder
