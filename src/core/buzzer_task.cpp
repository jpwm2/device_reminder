#include "buzzer_task/buzzer_task.hpp"
#include "process_message_operation/process_message.hpp"

namespace device_reminder {

BuzzerTask::BuzzerTask(std::shared_ptr<IBuzzerDriver> driver,
                       std::shared_ptr<ITimerService> timer,
                       std::shared_ptr<ILogger> logger)
    : driver_(std::move(driver))
    , timer_(std::move(timer))
    , logger_(std::move(logger))
{
    if (logger_) logger_->info("BuzzerTask created");
}

bool BuzzerTask::send_message(const IThreadMessage& msg) {
    onMessage(msg);
    return true;
}

void BuzzerTask::onMessage(const IThreadMessage& msg) {
    switch (msg.type()) {
    case MessageType::BuzzerOn:
        if (state_ == State::WaitStart) startBuzzer();
        break;
    case MessageType::BuzzerOff:
        if (state_ == State::Buzzing) stopBuzzer(true);
        break;
    case MessageType::Timeout:
        if (state_ == State::Buzzing) stopBuzzer(false);
        break;
    default:
        break;
    }
}

void BuzzerTask::startBuzzer() {
    if (driver_) driver_->start();
    if (timer_) timer_->start(kBuzzDurationMs, ProcessMessage{MessageType::Timeout});
    state_ = State::Buzzing;
}

void BuzzerTask::stopBuzzer(bool cancelTimer) {
    if (driver_) driver_->stop();
    if (cancelTimer && timer_) timer_->stop();
    state_ = State::WaitStart;
}

} // namespace device_reminder
