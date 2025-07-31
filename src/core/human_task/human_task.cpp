#include "human_task/human_task.hpp"

namespace device_reminder {

HumanTask::HumanTask(std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IPIRDriver> pir,
                     std::shared_ptr<IProcessSender> sender)
    : logger_(std::move(logger)), pir_(std::move(pir)), sender_(std::move(sender)) {
    if (logger_) logger_->info("HumanTask created");
}

void HumanTask::on_detecting(const std::vector<std::string>&) {
    if (!pir_) return;
    int val = pir_->read();
    if (val > 0 && sender_) {
        sender_->send();
        if (logger_) logger_->info("Human detected");
    }
}

void HumanTask::on_stopping(const std::vector<std::string>&) {
    if (logger_) logger_->info("Human detection stopped");
}

void HumanTask::on_cooldown(const std::vector<std::string>&) {
    // intentionally do nothing
    if (logger_) logger_->info("Human detection cooldown");
}

} // namespace device_reminder
