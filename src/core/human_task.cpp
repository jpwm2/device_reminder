#include "human_task/human_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "process_message_operation/process_message.hpp"

namespace device_reminder {

HumanTask::HumanTask(std::shared_ptr<IPIRDriver> pir,
                     std::shared_ptr<ITimerService> timer,
                     std::shared_ptr<IProcessMessageSender> sender,
                     std::shared_ptr<ILogger> logger)
    : pir_(std::move(pir))
    , timer_(std::move(timer))
    , sender_(std::move(sender))
    , logger_(std::move(logger))
    , state_(State::Detecting)
{
    if (logger_) logger_->info("HumanTask created");
}

HumanTask::~HumanTask() {
    if (timer_ && timer_->active()) timer_->stop();
    if (logger_) logger_->info("HumanTask destroyed");
}

void HumanTask::run(const IMessage& msg) {
    switch (msg.type()) {
    case MessageType::HumanDetected:
        if (state_ == State::Detecting) {
            if (sender_) sender_->enqueue(ProcessMessage{MessageType::HumanDetected, true});
            if (logger_) logger_->info("Human detected");
        }
        break;
    case MessageType::HumanDetectStop:
        if (state_ == State::Detecting) {
            state_ = State::Stopped;
            if (logger_) logger_->info("Detection stopped");
        }
        break;
    case MessageType::HumanDetectStart:
        if (state_ == State::Stopped) {
            state_ = State::Cooldown;
            if (timer_) timer_->start(5000, ProcessMessage{MessageType::Timeout});
            if (logger_) logger_->info("Detection cooldown");
        }
        break;
    case MessageType::Timeout:
        if (state_ == State::Cooldown) {
            state_ = State::Detecting;
            if (logger_) logger_->info("Cooldown end; detecting");
        }
        break;
    default:
        break;
    }
}

} // namespace device_reminder
