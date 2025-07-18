#include "gpio_driver.hpp"
#include <gpiod.h>
#include <stdexcept>
#include <iostream>

namespace device_reminder {

GPIODriver::GPIODriver()
    : chip_(nullptr), line_(nullptr) {}

GPIODriver::~GPIODriver() {
    close();
}

void GPIODriver::openChip(const std::string& chipName) {
    chip_ = gpiod_chip_open(chipName.c_str());
    if (!chip_) {
        throw std::runtime_error("Failed to open GPIO chip: " + chipName);
    }
}

void GPIODriver::setupLine(unsigned int lineNumber) {
    if (!chip_) {
        throw std::runtime_error("GPIO chip not opened");
    }
    line_ = gpiod_chip_get_line(chip_, lineNumber);
    if (!line_) {
        throw std::runtime_error("Failed to get GPIO line: " + std::to_string(lineNumber));
    }
    if (gpiod_line_request_input(line_, "pir-sensor") < 0) {
        throw std::runtime_error("Failed to request line as input");
    }
}

int GPIODriver::readLine() {
    if (!line_) {
        throw std::runtime_error("GPIO line not configured");
    }
    int value = gpiod_line_get_value(line_);
    if (value < 0) {
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
}

} // namespace device_reminder
