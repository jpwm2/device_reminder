#include "human_task/human_handler.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"

namespace device_reminder {

HumanHandler::HumanHandler(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IHumanTask> task,
                           std::shared_ptr<ITimerService> timer)
    : logger_(std::move(logger)), task_(std::move(task)), timer_(std::move(timer)) {
    if (logger_) logger_->info("HumanHandler created");
}

void HumanHandler::handle(std::shared_ptr<IProcessMessage> msg) {
    if (!msg || !task_) return;

    switch (msg->type()) {
    case ProcessMessageType::StartHumanDetection:
        state_ = State::Detecting;
        if (timer_) timer_->stop();
        if (logger_) logger_->info("StartHumanDetection");
        break;
    case ProcessMessageType::StopHumanDetection:
        state_ = State::Stopping;
        if (timer_) timer_->start();
        if (logger_) logger_->info("StopHumanDetection");
        break;
    case ProcessMessageType::CooldownTimeout:
        state_ = State::Cooldown;
        if (timer_) timer_->stop();
        if (logger_) logger_->info("CooldownTimeout");
        break;
    default:
        break;
    }

    switch (state_) {
    case State::Detecting:
        task_->on_detecting(msg->payload());
        break;
    case State::Stopping:
        task_->on_stopping(msg->payload());
        break;
    case State::Cooldown:
        task_->on_cooldown(msg->payload());
        break;
    }
}

} // namespace device_reminder
