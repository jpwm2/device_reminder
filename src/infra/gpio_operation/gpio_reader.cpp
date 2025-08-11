#include "infra/gpio_operation/gpio_reader.hpp"

#include <gpiod.h>
#include <stdexcept>
#include <string>

namespace device_reminder {

GPIOReader::GPIOReader(std::shared_ptr<ILogger> logger,
                       int pin_no,
                       std::shared_ptr<IFileLoader> loader)
    : logger_(std::move(logger)),
      pin_no_(pin_no),
      loader_(std::move(loader)),
      chip_(nullptr),
      line_(nullptr) {
    chip_ = gpiod_chip_open("/dev/gpiochip0");
    if (!chip_) {
        if (logger_) logger_->error("Failed to open GPIO chip: /dev/gpiochip0");
        throw std::runtime_error("Failed to open GPIO chip: /dev/gpiochip0");
    }
    line_ = gpiod_chip_get_line(chip_, static_cast<unsigned int>(pin_no_));
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
    if (logger_) logger_->info("GPIOReader::read start: pin=" + std::to_string(pin_no_));
    try {
        int value = gpiod_line_get_value(line_);
        if (value < 0) {
            throw std::runtime_error("Failed to read GPIO value");
        }
        if (value != 0 && value != 1) {
            throw std::runtime_error("Invalid GPIO value: " + std::to_string(value));
        }
        bool result = (value == 1);
        if (logger_) logger_->info(std::string("GPIOReader::read success: value=") + (result ? "true" : "false"));
        return result;
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("GPIOReader::read failed: ") + e.what());
        throw;
    }
}

void GPIOReader::poll_edge(bool detect_rising) {
    if (logger_)
        logger_->info(std::string("GPIOReader::poll_edge start: detect_rising=") + (detect_rising ? "true" : "false"));
    try {
        gpiod_line_release(line_);

        int ret = detect_rising ? gpiod_line_request_rising_edge_events(line_, "device_reminder")
                                : gpiod_line_request_falling_edge_events(line_, "device_reminder");
        if (ret < 0) {
            throw std::runtime_error("Failed to request edge events");
        }

        ret = gpiod_line_event_wait(line_, nullptr);
        if (ret < 0) {
            throw std::runtime_error("Waiting for edge event failed");
        }

        if (logger_) logger_->info(std::string("GPIOReader::poll_edge success: detected ") + (detect_rising ? "rising" : "falling") + " edge");

        gpiod_line_release(line_);
        if (gpiod_line_request_input(line_, "device_reminder") < 0) {
            throw std::runtime_error("Failed to re-request GPIO input line");
        }
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("GPIOReader::poll_edge failed: ") + e.what());
        throw;
    }
}

} // namespace device_reminder

