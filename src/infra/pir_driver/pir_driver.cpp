#include "pir_driver/pir_driver.hpp"
#include <stdexcept>

namespace device_reminder {

PIRDriver::PIRDriver(IGPIOReader* gpio_driver, ILogger* logger)
    : gpio_(gpio_driver), logger_(logger)
{
    if (logger_) {
        logger_->info("PIRDriver initialized");
    }
}

PIRDriver::~PIRDriver() {
    if (logger_) {
        logger_->info("PIRDriver shutting down");
    }
}

int PIRDriver::read() {
    try {
        return gpio_->read() ? 1 : 0;
    } catch (const std::exception& ex) {
        if (logger_) {
            logger_->error("Failed to read from PIR sensor: " + std::string(ex.what()));
        }
        return -1; // エラー時に-1を返す設計（用途に応じて調整可）
    }
}

} // namespace device_reminder
