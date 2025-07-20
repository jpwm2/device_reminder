#include "gpio_driver.hpp"
#include <gpiod.h>
#include <stdexcept>
#include <iostream>
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

GPIODriver::GPIODriver(std::shared_ptr<ILogger> logger)
    : chip_(nullptr), line_(nullptr), logger_(std::move(logger)) {
    if (logger_) logger_->info("GPIODriver created");
}

GPIODriver::~GPIODriver() {
    close();
    if (logger_) logger_->info("GPIODriver destroyed");
}

void GPIODriver::openChip(const std::string& chipName) {
    chip_ = gpiod_chip_open(chipName.c_str());
    if (!chip_) {
        if (logger_) logger_->error("Failed to open GPIO chip: " + chipName);
        throw std::runtime_error("Failed to open GPIO chip: " + chipName);
    }
}

void GPIODriver::setupLine(unsigned int lineNumber) {
    if (!chip_) {
        if (logger_) logger_->error("GPIO chip not opened");
        throw std::runtime_error("GPIO chip not opened");
    }
    line_ = gpiod_chip_get_line(chip_, lineNumber);
    if (!line_) {
        if (logger_) logger_->error("Failed to get GPIO line: " + std::to_string(lineNumber));
        throw std::runtime_error("Failed to get GPIO line: " + std::to_string(lineNumber));
    }
    if (gpiod_line_request_input(line_, "pir-sensor") < 0) {
        if (logger_) logger_->error("Failed to request line as input");
        throw std::runtime_error("Failed to request line as input");
    }
}

int GPIODriver::readLine() {
    if (!line_) {
        if (logger_) logger_->error("GPIO line not configured");
        throw std::runtime_error("GPIO line not configured");
    }
    int value = gpiod_line_get_value(line_);
    if (value < 0) {
        if (logger_) logger_->error("Failed to read GPIO line value");
        throw std::runtime_error("Failed to read GPIO line value");
    }
    return value;
}

void GPIODriver::close() {
    if (line_) {
        gpiod_line_release(line_);
        line_ = nullptr;
    }
    if (chip_) {
        gpiod_chip_close(chip_);
        chip_ = nullptr;
    }
    if (logger_) logger_->info("GPIODriver closed");
}

} // namespace device_reminder
