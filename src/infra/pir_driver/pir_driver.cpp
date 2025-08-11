#include "pir_driver/pir_driver.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace device_reminder {

PIRDriver::PIRDriver(std::shared_ptr<IFileLoader> loader,
                     std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IGPIOReader> gpio)
    : loader_(std::move(loader)),
      logger_(std::move(logger)),
      gpio_(std::move(gpio)) {}

void PIRDriver::run() {
    if (logger_) logger_->info("PIRDriver::run start");

    if (monitoring_) {
        if (logger_) logger_->error("PIRDriver::run already running");
        throw std::runtime_error("PIRDriver already running");
    }

    monitoring_ = true;
    interrupt_ = false;

    try {
        bool initial = gpio_->read();
        if (initial) {
            monitoring_ = false;
            if (logger_) logger_->info("PIRDriver::run success: already detected");
            return;
        }
    } catch (const std::exception& e) {
        monitoring_ = false;
        if (logger_) logger_->error(std::string("PIRDriver::run failed: ") + e.what());
        throw;
    }

    try {
        while (!interrupt_) {
            try {
                gpio_->poll_edge(true);
                if (interrupt_) break;
                monitoring_ = false;
                if (logger_) logger_->info("PIRDriver::run success: detected rising edge");
                return;
            } catch (const std::exception& e) {
                monitoring_ = false;
                if (logger_) logger_->error(std::string("PIRDriver::run failed: ") + e.what());
                throw;
            }
        }
    } catch (...) {
        monitoring_ = false;
        throw;
    }

    monitoring_ = false;
    if (logger_) logger_->info("PIRDriver::run success");
}

void PIRDriver::stop() {
    if (logger_) logger_->info("PIRDriver::stop start");

    if (!monitoring_) {
        if (logger_) logger_->info("PIRDriver::stop already stopped");
        return;
    }

    interrupt_ = true;

    try {
        // リソース解放処理が必要な場合はここで行う
    } catch (const std::exception& e) {
        monitoring_ = false;
        if (logger_) logger_->error(std::string("PIRDriver::stop failed: ") + e.what());
        throw;
    }

    monitoring_ = false;
    if (logger_) logger_->info("PIRDriver::stop success");
}

} // namespace device_reminder

