#include "infra/gpio_operation/gpio_reader/gpio_reader.hpp"

#include <gpiod.h>
#include <stdexcept>

namespace device_reminder {

GPIOReader::GPIOReader(std::shared_ptr<ILogger> logger,
                       int pin_no,
                       std::string chip_name)
    : logger_(std::move(logger)),
      pin_no_(pin_no),
      chip_name_(std::move(chip_name)),
      chip_(nullptr),
      line_(nullptr) {
    chip_ = gpiod_chip_open(chip_name_.c_str());
    if (!chip_) {
        if (logger_) logger_->error("Failed to open GPIO chip: " + chip_name_);
        throw std::runtime_error("Failed to open GPIO chip: " + chip_name_);
    }
    line_ = gpiod_chip_get_line(chip_, pin_no_);
    if (!line_) {
        if (logger_) logger_->error("Failed to get GPIO line: " + std::to_string(pin_no_));
        gpiod_chip_close(chip_);
        throw std::runtime_error("Failed to get GPIO line");
    }
    if (gpiod_line_request_input(line_, "device_reminder") < 0) {
        if (logger_) logger_->error("Failed to request GPIO input line");
        gpiod_chip_close(chip_);
        line_ = nullptr;
        chip_ = nullptr;
        throw std::runtime_error("Failed to request GPIO input line");
    }
}

GPIOReader::~GPIOReader() {
    if (line_) {
        gpiod_line_release(line_);
        line_ = nullptr;
    }
    if (chip_) {
        gpiod_chip_close(chip_);
        chip_ = nullptr;
    }
}

bool GPIOReader::read() {
    int value = gpiod_line_get_value(line_);
    if (value < 0) {
        if (logger_) logger_->error("Failed to read GPIO value");
        throw std::runtime_error("Failed to read GPIO value");
    }
    return value != 0;
}

} // namespace device_reminder
