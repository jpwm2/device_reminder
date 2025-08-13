#include "buzzer_task/buzzer_handler.hpp"
#include "infra/message/thread_sender.hpp"

#include <utility>

namespace device_reminder {

BuzzerHandler::BuzzerHandler(std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IFileLoader> file_loader,
                             std::shared_ptr<IBuzzerDriver> driver,
                             std::shared_ptr<ITimerService> timer_service,
                             std::shared_ptr<IMessageQueue> buzzer_queue,
                             std::shared_ptr<IMessage> buzzing_end_msg)
    : logger_(std::move(logger))
    , file_loader_(std::move(file_loader))
    , driver_(std::move(driver))
    , timer_service_(std::move(timer_service))
    , buzzer_queue_(std::move(buzzer_queue))
    , buzzing_end_msg_(std::move(buzzing_end_msg)) {}

void BuzzerHandler::start_buzzing_and_start_timer() {
    if (logger_) logger_->info("[BuzzerHandler::start_buzzing_and_start_timer] start");
    try {
        if (driver_) {
            driver_->on();
        }

        if (timer_service_ && buzzer_queue_ && buzzing_end_msg_) {
            auto thread_sender = std::make_shared<ThreadSender>(logger_);
            timer_service_->start(0, thread_sender, buzzer_queue_, buzzing_end_msg_);
        }

        if (logger_) logger_->info("[BuzzerHandler::start_buzzing_and_start_timer] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[BuzzerHandler::start_buzzing_and_start_timer] failed: ") + e.what());
        throw;
    }
}

void BuzzerHandler::stop_buzzing_and_stop_timer() {
    if (logger_) logger_->info("[BuzzerHandler::stop_buzzing_and_stop_timer] start");
    try {
        if (driver_) {
            driver_->off();
        }

        if (timer_service_) {
            timer_service_->stop();
        }

        if (logger_) logger_->info("[BuzzerHandler::stop_buzzing_and_stop_timer] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[BuzzerHandler::stop_buzzing_and_stop_timer] failed: ") + e.what());
        throw;
    }
}

void BuzzerHandler::stop_buzzing() {
    if (logger_) logger_->info("[BuzzerHandler::stop_buzzing] start");
    try {
        if (driver_) {
            driver_->off();
        }

        if (logger_) logger_->info("[BuzzerHandler::stop_buzzing] success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[BuzzerHandler::stop_buzzing] failed: ") + e.what());
        throw;
    }
}

} // namespace device_reminder

