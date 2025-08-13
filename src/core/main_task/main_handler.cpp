#include "main_task/main_handler.hpp"
#include "infra/message/thread_sender.hpp"

namespace device_reminder {

MainHandler::MainHandler(std::shared_ptr<ILogger> logger,
                         std::shared_ptr<IProcessSender> sender,
                         std::shared_ptr<IFileLoader> file_loader,
                         std::shared_ptr<ITimerService> timer_service,
                         std::shared_ptr<IMessageQueue> human_queue,
                         std::shared_ptr<IMessage> human_start_msg,
                         std::shared_ptr<IMessage> human_stop_msg,
                         std::shared_ptr<IMessageQueue> bt_queue,
                         std::shared_ptr<IMessage> device_scan_msg,
                         std::shared_ptr<IMessageQueue> buzzer_queue,
                         std::shared_ptr<IMessage> buzzer_start_msg,
                         std::shared_ptr<IMessage> buzzer_stop_msg,
                         std::shared_ptr<IMessageQueue> main_queue,
                         std::shared_ptr<IMessage> bt_cooldown_end_msg,
                         std::shared_ptr<IMessage> device_detect_timeout_msg)
    : logger_(std::move(logger))
    , sender_(std::move(sender))
    , file_loader_(std::move(file_loader))
    , timer_service_(std::move(timer_service))
    , human_queue_(std::move(human_queue))
    , human_start_msg_(std::move(human_start_msg))
    , human_stop_msg_(std::move(human_stop_msg))
    , bt_queue_(std::move(bt_queue))
    , device_scan_msg_(std::move(device_scan_msg))
    , buzzer_queue_(std::move(buzzer_queue))
    , buzzer_start_msg_(std::move(buzzer_start_msg))
    , buzzer_stop_msg_(std::move(buzzer_stop_msg))
    , main_queue_(std::move(main_queue))
    , bt_cooldown_end_msg_(std::move(bt_cooldown_end_msg))
    , device_detect_timeout_msg_(std::move(device_detect_timeout_msg)) {}

void MainHandler::start_device_detect() {
    if (logger_) logger_->info("[MainHandler::start_device_detect] start");
    try {
        if (sender_ && bt_queue_ && device_scan_msg_) {
            sender_->send(bt_queue_, device_scan_msg_);
        }
        if (sender_ && human_queue_ && human_stop_msg_) {
            sender_->send(human_queue_, human_stop_msg_);
        }
        if (timer_service_ && main_queue_ && device_detect_timeout_msg_) {
            auto thread_sender = std::make_shared<ThreadSender>(logger_);
            timer_service_->start(0, thread_sender, main_queue_, device_detect_timeout_msg_);
        }
        if (logger_) logger_->info("[MainHandler::start_device_detect] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[MainHandler::start_device_detect] failed: ") + e.what());
        throw;
    }
}

void MainHandler::end_device_first_detect() {
    if (logger_) logger_->info("[MainHandler::end_device_first_detect] start");
    try {
        if (sender_ && human_queue_ && human_start_msg_) {
            sender_->send(human_queue_, human_start_msg_);
        }
        if (logger_) logger_->info("[MainHandler::end_device_first_detect] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[MainHandler::end_device_first_detect] failed: ") + e.what());
        throw;
    }
}

void MainHandler::end_device_retry_detect() {
    if (logger_) logger_->info("[MainHandler::end_device_retry_detect] start");
    try {
        if (sender_ && human_queue_ && human_start_msg_) {
            sender_->send(human_queue_, human_start_msg_);
        }
        if (sender_ && buzzer_queue_ && buzzer_stop_msg_) {
            sender_->send(buzzer_queue_, buzzer_stop_msg_);
        }
        if (logger_) logger_->info("[MainHandler::end_device_retry_detect] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[MainHandler::end_device_retry_detect] failed: ") + e.what());
        throw;
    }
}

void MainHandler::retry_device_detect() {
    if (logger_) logger_->info("[MainHandler::retry_device_detect] start");
    try {
        if (sender_ && buzzer_queue_ && buzzer_start_msg_) {
            sender_->send(buzzer_queue_, buzzer_start_msg_);
        }
        if (timer_service_ && main_queue_ && bt_cooldown_end_msg_) {
            auto thread_sender = std::make_shared<ThreadSender>(logger_);
            timer_service_->start(0, thread_sender, main_queue_, bt_cooldown_end_msg_);
        }
        if (logger_) logger_->info("[MainHandler::retry_device_detect] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[MainHandler::retry_device_detect] failed: ") + e.what());
        throw;
    }
}

void MainHandler::send_bt_request() {
    if (logger_) logger_->info("[MainHandler::send_bt_request] start");
    try {
        if (sender_ && bt_queue_ && device_scan_msg_) {
            sender_->send(bt_queue_, device_scan_msg_);
        }
        if (logger_) logger_->info("[MainHandler::send_bt_request] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[MainHandler::send_bt_request] failed: ") + e.what());
        throw;
    }
}

} // namespace device_reminder

