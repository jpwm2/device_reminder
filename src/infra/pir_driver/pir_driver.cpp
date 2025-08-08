#include "pir_driver/pir_driver.hpp"

#include <utility>

namespace device_reminder {

PIRDriver::PIRDriver(std::shared_ptr<IFileLoader> gpiochip_loader,
                     std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IThreadSender> sender,
                     std::shared_ptr<IGPIOReader> reader,
                     IFileLoader& pin_loader)
    : gpiochip_loader_(std::move(gpiochip_loader))
    , logger_(std::move(logger))
    , sender_(std::move(sender))
    , reader_(std::move(reader))
    , pin_loader_(pin_loader) {}

PIRDriver::~PIRDriver() { stop(); }

void PIRDriver::run() {
    if (running_ || !reader_) { return; }

    try {
        gpiochip_loader_->load_string();
        pin_loader_.load_int();
    } catch (const std::exception& e) {
        if (logger_) { logger_->error("PIRDriver run failed: " + std::string(e.what())); }
        throw;
    }

    running_ = true;
    thread_exception_ = nullptr;
    thread_ = std::thread([this]() {
        try {
            reader_->poll_edge(true);
            if (running_) {
                if (logger_) { logger_->info("人感センサ検出"); }
                if (sender_) { sender_->send(); }
            }
        } catch (const std::exception& e) {
            if (logger_) { logger_->error("PIRDriver poll_edge error: " + std::string(e.what())); }
            thread_exception_ = std::current_exception();
        }
        running_ = false;
    });

    if (logger_) { logger_->info("PIRDriver開始"); }
}

void PIRDriver::stop() {
    running_ = false;
    if (thread_.joinable()) { thread_.join(); }
    if (logger_) { logger_->info("PIRDriver停止"); }
    if (thread_exception_) { std::rethrow_exception(thread_exception_); }
}

} // namespace device_reminder

