#include "human_task/human_task.hpp"

namespace device_reminder {

HumanTask::HumanTask(std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IPIRDriver> pir,
                     std::shared_ptr<IProcessSender> sender)
    : logger_(std::move(logger)), pir_(std::move(pir)), sender_(std::move(sender)) {
    if (logger_) logger_->info("HumanTask created");
}

void HumanTask::on_detecting(const std::vector<std::string>&) {
    if (pir_) {
        pir_->run();
    }
    if (logger_) logger_->info("Human detection started");
}

void HumanTask::on_stopping(const std::vector<std::string>&) {
    if (pir_) {
        pir_->stop();
    }
    if (logger_) logger_->info("Human detection stopped");
}

void HumanTask::on_cooldown(const std::vector<std::string>&) {
    if (pir_) {
        pir_->stop();
    }
    if (logger_) logger_->info("Human detection cooldown");
}

} // namespace device_reminder
