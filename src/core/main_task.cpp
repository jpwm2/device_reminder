#include "main_task/main_task.hpp"
#include "process_message_operation/process_message.hpp"

namespace device_reminder {

MainTask::MainTask(std::shared_ptr<ITimerService> det_timer,
                   std::shared_ptr<ITimerService> cooldown_timer,
                   std::shared_ptr<IProcessMessageSender> human_sender,
                   std::shared_ptr<IProcessMessageSender> bluetooth_sender,
                   std::shared_ptr<IProcessMessageSender> buzzer_sender,
                   std::shared_ptr<ILogger> logger)
    : det_timer_(std::move(det_timer))
    , cooldown_timer_(std::move(cooldown_timer))
    , human_sender_(std::move(human_sender))
    , bluetooth_sender_(std::move(bluetooth_sender))
    , buzzer_sender_(std::move(buzzer_sender))
    , logger_(std::move(logger))
{
    if (logger_) logger_->info("MainTask created");
}

MainTask::~MainTask() {
    if (logger_) logger_->info("MainTask destroyed");
}

void MainTask::run(const IThreadMessage& msg) {
    auto type = msg.type();
    switch (state_) {
    case State::WaitHumanDetect:
        if (type == MessageType::HumanDetected) {
            if (det_timer_)
                det_timer_->start(4000,
                                  ProcessMessage{MessageType::Timeout,
                                          static_cast<bool>(TimerId::T_DET_TIMEOUT)});
            if (bluetooth_sender_)
                bluetooth_sender_->enqueue(ProcessMessage{MessageType::StartScan});
            state_ = State::WaitDeviceResponse;
            if (logger_) logger_->info("Human detected -> wait device response");
        }
        break;
    case State::WaitDeviceResponse:
        if (type == MessageType::DeviceScanResult) {
            if (msg.payload()) {
                if (human_sender_)
                    human_sender_->enqueue(ProcessMessage{MessageType::HumanDetectStart});
                if (buzzer_sender_)
                    buzzer_sender_->enqueue(ProcessMessage{MessageType::BuzzerOff});
                if (det_timer_ && det_timer_->active()) det_timer_->stop();
                state_ = State::WaitHumanDetect;
            } else {
                if (cooldown_timer_)
                    cooldown_timer_->start(1000,
                                           ProcessMessage{MessageType::Timeout,
                                                   static_cast<bool>(TimerId::T_COOLDOWN)});
                state_ = State::ScanCooldown;
            }
        } else if (type == MessageType::Timeout &&
                   msg.payload() == static_cast<bool>(TimerId::T_DET_TIMEOUT)) {
            state_ = State::WaitHumanDetect;
        }
        break;
    case State::ScanCooldown:
        if (type == MessageType::Timeout) {
            if (msg.payload() == static_cast<bool>(TimerId::T_COOLDOWN)) {
                if (bluetooth_sender_)
                bluetooth_sender_->enqueue(ProcessMessage{MessageType::StartScan});
                state_ = State::WaitDeviceResponse;
            } else if (msg.payload() == static_cast<bool>(TimerId::T_DET_TIMEOUT)) {
                state_ = State::WaitHumanDetect;
            }
        }
        break;
    }
}

} // namespace device_reminder
