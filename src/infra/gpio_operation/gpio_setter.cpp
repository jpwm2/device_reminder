#include "gpio_operation/gpio_setter/gpio_setter.hpp"
#include <gpiod.h>
#include <stdexcept>

namespace device_reminder {

GPIOSetter::GPIOSetter(std::shared_ptr<ILogger> logger,
                       int pin_no,
                       std::string chip_name)
    : logger_(std::move(logger)), chip_(nullptr), line_(nullptr) {
    chip_ = gpiod_chip_open(chip_name.c_str());
    if (!chip_) {
        if (logger_) logger_->error("Failed to open GPIO chip: " + chip_name);
        throw std::runtime_error("Failed to open GPIO chip: " + chip_name);
    }

    line_ = gpiod_chip_get_line(chip_, static_cast<unsigned int>(pin_no));
    if (!line_) {
        if (logger_) logger_->error("Failed to get GPIO line: " + std::to_string(pin_no));
        gpiod_chip_close(chip_);
        throw std::runtime_error("Failed to get GPIO line");
    }

    if (gpiod_line_request_output(line_, "device_reminder", 0) < 0) {
        if (logger_) logger_->error("Failed to request GPIO line");
        gpiod_chip_close(chip_);
        line_ = nullptr;
        chip_ = nullptr;
        throw std::runtime_error("Failed to request GPIO line");
    }

    if (logger_) logger_->info("GPIOSetter initialized");
}

GPIOSetter::~GPIOSetter() {
    if (line_) {
        gpiod_line_release(line_);
        line_ = nullptr;
    }
    if (chip_) {
        gpiod_chip_close(chip_);
        chip_ = nullptr;
    }
    if (logger_) logger_->info("GPIOSetter destroyed");
}

void GPIOSetter::write(bool value) {
    if (!line_) {
        if (logger_) logger_->error("GPIO line not initialized");
        throw std::runtime_error("GPIO line not initialized");
    }
    if (gpiod_line_set_value(line_, value ? 1 : 0) < 0) {
        if (logger_) logger_->error("Failed to write GPIO line value");
        throw std::runtime_error("Failed to write GPIO line value");
    }
}

} // namespace device_reminder
