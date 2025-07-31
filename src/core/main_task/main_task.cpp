#include "main_task/main_task.hpp"
#include "infra/process_operation/process_message/process_message.hpp"
#include <algorithm>

namespace device_reminder {

MainTask::MainTask(std::shared_ptr<ILogger> logger,
                   std::shared_ptr<IFileLoader> file_loader,
                   std::shared_ptr<IProcessSender> human_start_sender,
                   std::shared_ptr<IProcessSender> human_stop_sender,
                   std::shared_ptr<IProcessSender> bluetooth_sender,
                   std::shared_ptr<IProcessSender> buzzer_start_sender,
                   std::shared_ptr<IProcessSender> buzzer_stop_sender,
                   std::shared_ptr<ITimerService> det_timer,
                   std::shared_ptr<ITimerService> cooldown_timer)
    : logger_(std::move(logger))
    , file_loader_(std::move(file_loader))
    , human_start_sender_(std::move(human_start_sender))
    , human_stop_sender_(std::move(human_stop_sender))
    , bluetooth_sender_(std::move(bluetooth_sender))
    , buzzer_start_sender_(std::move(buzzer_start_sender))
    , buzzer_stop_sender_(std::move(buzzer_stop_sender))
    , det_timer_(std::move(det_timer))
    , cooldown_timer_(std::move(cooldown_timer))
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
        if (type == ThreadMessageType::HumanDetected) {
            on_waiting_for_human(msg.payload());
        }
        break;
    case State::WaitDeviceResponse:
        if (type == ThreadMessageType::RespondDeviceFound) {
            on_response_to_human_task(msg.payload());
        } else if (type == ThreadMessageType::RespondDeviceNotFound) {
            on_response_to_buzzer_task(msg.payload());
        } else if (type == ThreadMessageType::ProcessingTimeout &&
                   !msg.payload().empty() &&
                   msg.payload()[0] == std::to_string(static_cast<int>(TimerId::T_DET_TIMEOUT))) {
            on_cooldown(msg.payload());
        }
        break;
    case State::ScanCooldown:
        if (type == ThreadMessageType::ProcessingTimeout) {
            if (!msg.payload().empty() &&
                msg.payload()[0] == std::to_string(static_cast<int>(TimerId::T_COOLDOWN))) {
                on_waiting_for_second_response(msg.payload());
            } else if (!msg.payload().empty() &&
                       msg.payload()[0] == std::to_string(static_cast<int>(TimerId::T_DET_TIMEOUT))) {
                on_cooldown(msg.payload());
            }
        }
        break;
    }
}

void MainTask::on_waiting_for_human(const std::vector<std::string>&) {
    if (det_timer_)
        det_timer_->start();
    if (bluetooth_sender_)
        bluetooth_sender_->send();
    state_ = State::WaitDeviceResponse;
    if (logger_) logger_->info("Human detected -> wait device response");
}

void MainTask::on_response_to_buzzer_task(const std::vector<std::string>& payload) {
    bool found = false;
    if (file_loader_) {
        auto regs = file_loader_->load_string_list();
        for (const auto& d : payload) {
            if (std::find(regs.begin(), regs.end(), d) != regs.end()) {
                found = true;
                break;
            }
        }
    }
    if (!found) {
        if (buzzer_start_sender_)
            buzzer_start_sender_->send();
        if (cooldown_timer_)
            cooldown_timer_->start();
        state_ = State::ScanCooldown;
    }
    if (logger_) logger_->info("Device not found -> cooldown");
}

void MainTask::on_response_to_human_task(const std::vector<std::string>& payload) {
    bool found = false;
    if (file_loader_) {
        auto regs = file_loader_->load_string_list();
        for (const auto& d : payload) {
            if (std::find(regs.begin(), regs.end(), d) != regs.end()) {
                found = true;
                break;
            }
        }
    }
    if (found) {
        if (human_start_sender_)
            human_start_sender_->send();
        if (buzzer_stop_sender_)
            buzzer_stop_sender_->send();
        if (det_timer_)
            det_timer_->stop();
        state_ = State::WaitHumanDetect;
        if (logger_) logger_->info("Device found -> resume human detect");
    } else {
        on_response_to_buzzer_task(payload);
    }
}

void MainTask::on_cooldown(const std::vector<std::string>&) {
    state_ = State::WaitHumanDetect;
    if (logger_) logger_->info("Timeout -> wait human detect");
}

void MainTask::on_waiting_for_second_response(const std::vector<std::string>& payload) {
    bool found = false;
    if (file_loader_) {
        auto regs = file_loader_->load_string_list();
        for (const auto& d : payload) {
            if (std::find(regs.begin(), regs.end(), d) != regs.end()) {
                found = true;
                break;
            }
        }
    }
    if (found) {
        if (human_start_sender_)
            human_start_sender_->send();
        if (buzzer_stop_sender_)
            buzzer_stop_sender_->send();
        state_ = State::WaitHumanDetect;
    } else {
        if (buzzer_start_sender_)
            buzzer_start_sender_->send();
        state_ = State::ScanCooldown;
    }
    if (logger_) logger_->info("Processed second scan result");
}

} // namespace device_reminder
