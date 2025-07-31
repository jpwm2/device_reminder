#include "buzzer_task/buzzer_handler.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"

namespace device_reminder {

BuzzerHandler::BuzzerHandler(std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IBuzzerTask> task,
                             std::shared_ptr<ITimerService> timer)
    : logger_(std::move(logger)), task_(std::move(task)), timer_(std::move(timer)) {
    if (logger_) logger_->info("BuzzerHandler created");
}

void BuzzerHandler::handle(std::shared_ptr<IProcessMessage> msg) {
    if (!msg || !task_) return;

    switch (msg->type()) {
    case ProcessMessageType::StartBuzzing:
        if (timer_) timer_->start();
        task_->on_buzzing(msg->payload());
        if (logger_) logger_->info("StartBuzzing");
        break;
    case ProcessMessageType::StopBuzzing:
    case ProcessMessageType::BuzzTimeout:
        if (timer_) timer_->stop();
        task_->on_waiting(msg->payload());
        if (logger_) {
            logger_->info(msg->type() == ProcessMessageType::StopBuzzing ?
                          "StopBuzzing" : "BuzzTimeout");
        }
        break;
    default:
        break;
    }
}

} // namespace device_reminder
