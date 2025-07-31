#include "pir_driver/pir_driver.hpp"
#include <chrono>
#include <thread>

namespace device_reminder {

PIRDriver::PIRDriver(std::shared_ptr<IFileLoader> loader,
                     std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IThreadSender> sender,
                     std::shared_ptr<IGPIOReader> gpio)
    : loader_(std::move(loader)),
      logger_(std::move(logger)),
      sender_(std::move(sender)),
      gpio_(std::move(gpio))
{
    if (logger_) {
        logger_->info("PIRDriver initialized");
    }
    if (gpio_) {
        try {
            last_state_ = gpio_->read();
        } catch (...) {
            last_state_ = false;
        }
    }
}

PIRDriver::~PIRDriver() {
    stop();
    if (logger_) {
        logger_->info("PIRDriver shutting down");
    }
}

void PIRDriver::run() {
    if (running_ || !gpio_) return;
    running_ = true;
    thread_ = std::thread([this]() {
        bool prev = last_state_;
        while (running_) {
            bool val = false;
            try {
                val = gpio_->read();
            } catch (...) {
                if (logger_) logger_->error("PIRDriver read error");
            }
            if (val != prev) {
                prev = val;
                last_state_ = val;
                if (sender_) sender_->send();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    if (logger_) logger_->info("PIRDriver started");
}

void PIRDriver::stop() {
    running_ = false;
    if (thread_.joinable()) thread_.join();
    if (logger_) logger_->info("PIRDriver stopped");
}


} // namespace device_reminder
