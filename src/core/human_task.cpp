#include "human_task.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

HumanTask::HumanTask(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {
    if (logger_) logger_->info("HumanTask created");
}

HumanTask::~HumanTask() {
    if (logger_) logger_->info("HumanTask destroyed");
}

void HumanTask::run() {
    if (logger_) logger_->info("HumanTask running");
}

bool HumanTask::send_message(const IMessage& msg) {
    if (logger_) logger_->info("HumanTask send_message");
    return true;
}

} // namespace device_reminder
