#include "RIPDriver.hpp"
#include "IGPIODriver.hpp"
#include "ILogger.hpp"
#include <stdexcept>

PIRDriver::PIRDriver(IGPIODriver* gpio_driver, ILogger* logger, unsigned int gpioLine, const std::string& chipName)
    : gpio_(gpio_driver), logger_(logger), chip_(nullptr), line_(nullptr) {

    chip_ = gpio_->openChip(chipName);
    if (!chip_) {
        logger_->error("Failed to open GPIO chip");
        throw std::runtime_error("Failed to open GPIO chip");
    }

    line_ = gpio_->getLine(chip_, gpioLine);
    if (!line_) {
        logger_->error("Failed to get GPIO line");
        gpio_->closeChip(chip_);
        throw std::runtime_error("Failed to get GPIO line");
    }

    if (gpio_->requestInput(line_, "pir_driver") < 0) {
        logger_->error("Failed to request line as input");
        gpio_->closeChip(chip_);
        throw std::runtime_error("Failed to request line as input");
    }

    logger_->info("PIRDriver initialized");
}

PIRDriver::~PIRDriver() {
    if (line_) gpio_->releaseLine(line_);
    if (chip_) gpio_->closeChip(chip_);
    logger_->info("PIRDriver cleaned up");
}

int PIRDriver::read() {
    int value = gpio_->getValue(line_);
    if (value < 0) {
        logger_->error("Failed to read GPIO value");
        throw std::runtime_error("Failed to read GPIO value");
    }
    return value;
}
