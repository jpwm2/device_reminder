#include "buzzer_task/buzzer_task.hpp"
#include "infra/process_operation/process_message/process_message.hpp"

namespace device_reminder {

BuzzerTask::BuzzerTask(std::shared_ptr<ILogger> logger,
                       std::shared_ptr<IProcessSender> sender,
                       std::shared_ptr<IFileLoader> file_loader,
                       std::shared_ptr<IBuzzerDriver> driver)
    : logger_(std::move(logger))
    , sender_(std::move(sender))
    , file_loader_(std::move(file_loader))
    , driver_(std::move(driver))
{
    if (logger_) logger_->info("BuzzerTask created");
}

bool BuzzerTask::send_message(const IThreadMessage& msg) {
    onMessage(msg);
    return true;
}

void BuzzerTask::onMessage(const IThreadMessage& msg) {
    switch (msg.type()) {
    case ThreadMessageType::StartBuzzing:
        if (state_ == State::WaitStart) startBuzzer();
        break;
    case ThreadMessageType::StopBuzzing:
        if (state_ == State::Buzzing) stopBuzzer();
        break;
    default:
        break;
    }
}

void BuzzerTask::startBuzzer() {
    if (driver_) driver_->on();
    state_ = State::Buzzing;
}

void BuzzerTask::stopBuzzer() {
    if (driver_) driver_->off();
    state_ = State::WaitStart;
}

void BuzzerTask::on_waiting(const std::vector<std::string>&) {
    startBuzzer();
}

void BuzzerTask::on_buzzing(const std::vector<std::string>&) {
    stopBuzzer();
}

} // namespace device_reminder
