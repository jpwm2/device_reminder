#include "gpio_driver.hpp"
#include <gpiod.h>
#include <stdexcept>

namespace device_reminder {

GPIODriver::GPIODriver(int pinNumber, Direction direction, ILogger* logger,
                       const std::string& chipName)
    : chip_(nullptr),
      line_(nullptr),
      logger_(logger),
      direction_(direction),
      pinNumber_(pinNumber),
      chipName_(chipName) {
    chip_ = gpiod_chip_open(chipName_.c_str());
    if (!chip_) {
        if (logger_) logger_->error("Failed to open GPIO chip: " + chipName_);
        throw std::runtime_error("Failed to open GPIO chip: " + chipName_);
    }
    line_ = gpiod_chip_get_line(chip_, pinNumber_);
    if (!line_) {
        if (logger_) logger_->error("Failed to get GPIO line: " + std::to_string(pinNumber_));
        gpiod_chip_close(chip_);
        throw std::runtime_error("Failed to get GPIO line");
    }
    int ret = 0;
    if (direction_ == Direction::INPUT) {
        ret = gpiod_line_request_input(line_, "device_reminder");
    } else {
        ret = gpiod_line_request_output(line_, "device_reminder", 0);
    }
    if (ret < 0) {
        if (logger_) logger_->error("Failed to request GPIO line");
        gpiod_chip_close(chip_);
        line_ = nullptr;
        chip_ = nullptr;
        throw std::runtime_error("Failed to request GPIO line");
    }
    if (logger_) logger_->info("GPIODriver initialized");
}

GPIODriver::~GPIODriver() {
    if (line_) {
        gpiod_line_release(line_);
        line_ = nullptr;
    }
    if (chip_) {
        gpiod_chip_close(chip_);
        chip_ = nullptr;
    }
    if (logger_) logger_->info("GPIODriver destroyed");
}

void GPIODriver::write(bool value) {
    if (direction_ != Direction::OUTPUT) {
        if (logger_) logger_->error("Attempt to write on input GPIO line");
        throw std::runtime_error("GPIO line not configured for output");
    }
    if (gpiod_line_set_value(line_, value ? 1 : 0) < 0) {
        if (logger_) logger_->error("Failed to write GPIO line value");
        throw std::runtime_error("Failed to write GPIO line value");
    }
}

bool GPIODriver::read() {
    int value = gpiod_line_get_value(line_);
    if (value < 0) {
        if (logger_) logger_->error("Failed to read GPIO line value");
        throw std::runtime_error("Failed to read GPIO line value");
    }
    return value != 0;
}

void GPIODriver::setEdge(EdgeType edge) {
    if (direction_ != Direction::INPUT) {
        if (logger_) logger_->error("Edge detection requires input direction");
        throw std::runtime_error("Edge detection requires input direction");
    }
    gpiod_line_release(line_);
    int ret = 0;
    switch (edge) {
        case EdgeType::RISING:
            ret = gpiod_line_request_rising_edge_events(line_, "device_reminder");
            break;
        case EdgeType::FALLING:
            ret = gpiod_line_request_falling_edge_events(line_, "device_reminder");
            break;
        case EdgeType::BOTH:
            ret = gpiod_line_request_both_edges_events(line_, "device_reminder");
            break;
    }
    if (ret < 0) {
        if (logger_) logger_->error("Failed to set edge detection");
        throw std::runtime_error("Failed to set edge detection");
    }
}

void GPIODriver::waitForEdge() {
    if (direction_ != Direction::INPUT) {
        if (logger_) logger_->error("Waiting for edge requires input direction");
        throw std::runtime_error("Waiting for edge requires input direction");
    }
    int ret = gpiod_line_event_wait(line_, nullptr);
    if (ret < 0) {
        if (logger_) logger_->error("Failed while waiting for edge");
        throw std::runtime_error("Failed while waiting for edge");
    }
}

} // namespace device_reminder

