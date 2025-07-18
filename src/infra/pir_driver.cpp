#include "pir_driver/pir_driver.hpp"
#include <stdexcept>

namespace device_reminder {

PIRDriver::PIRDriver(IGPIODriver* gpio_driver, ILogger* logger,
                     unsigned int gpioLine, const std::string& chipName)
    : gpio_(gpio_driver), logger_(logger)
{
    try {
        gpio_->openChip(chipName);
        gpio_->setupLine(gpioLine);
        if (logger_) {
            logger_->info("PIRDriver initialized: line " + std::to_string(gpioLine));
        }
    } catch (const std::exception& ex) {
        if (logger_) {
            logger_->error("PIRDriver initialization failed: " + std::string(ex.what()));
        }
        throw; // 例外を呼び出し元に投げ返す
    }
}

PIRDriver::~PIRDriver() {
    if (logger_) {
        logger_->info("PIRDriver shutting down");
    }
    gpio_->close(); // リソース解放
}

int PIRDriver::read() {
    try {
        return gpio_->readLine();
    } catch (const std::exception& ex) {
        if (logger_) {
            logger_->error("Failed to read from PIR sensor: " + std::string(ex.what()));
        }
        return -1; // エラー時に-1を返す設計（用途に応じて調整可）
    }
}

} // namespace device_reminder
