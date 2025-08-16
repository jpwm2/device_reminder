#include "infra/gpio_operation/gpio_setter.hpp"

#include <gpiod.h>
#include <stdexcept>
#include <string>

namespace device_reminder {

GPIOSetter::GPIOSetter(std::shared_ptr<ILogger>     logger,
                       int                          pin_no,
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

void GPIOSetter::write(bool is_high) {
    if (logger_)
        logger_->info("GPIOSetter::write start: pin=" + std::to_string(pin_no_) +
                       ", value=" + (is_high ? "true" : "false"));
    try {
        if (!line_) {
            throw std::runtime_error("GPIO line not initialized");
        }
        int normalized = is_high ? 1 : 0;
        if (gpiod_line_set_value(line_, normalized) < 0) {
            throw std::runtime_error("Failed to write GPIO line value");
        }
        if (logger_)
            logger_->info(std::string("GPIOSetter::write success: value=") +
                           (normalized ? "1" : "0"));
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("GPIOSetter::write failed: ") + e.what());
        throw;
    }
}

void GPIOSetter::buzz_start(int frequency_hz) {
    if (logger_)
        logger_->info("GPIOSetter::buzz_start start: pin=" + std::to_string(pin_no_) +
                       ", frequency=" + std::to_string(frequency_hz));
    try {
        if (!line_) {
            throw std::runtime_error("GPIO line not initialized");
        }
        if (frequency_hz <= 0) {
            throw std::invalid_argument("frequency must be greater than 0");
        }
        if (gpiod_line_set_value(line_, 1) < 0) {
            throw std::runtime_error("Failed to start buzzer");
        }
        if (logger_)
            logger_->info("GPIOSetter::buzz_start success: frequency=" +
                           std::to_string(frequency_hz));
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("GPIOSetter::buzz_start failed: ") + e.what());
        throw;
    }
}

void GPIOSetter::buzz_stop() {
    if (logger_)
        logger_->info("GPIOSetter::buzz_stop start: pin=" + std::to_string(pin_no_));
    try {
        if (!line_) {
            throw std::runtime_error("GPIO line not initialized");
        }
        if (gpiod_line_set_value(line_, 0) < 0) {
            throw std::runtime_error("Failed to stop buzzer");
        }

        int value = gpiod_line_get_value(line_);
        if (value < 0) {
            throw std::runtime_error("Failed to read buzzer state");
        }
        if (value != 0) {
            throw std::runtime_error("Buzzer did not stop");
        }

        if (logger_)
            logger_->info(std::string("GPIOSetter::buzz_stop success: value=") +
                           (value ? "1" : "0"));
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("GPIOSetter::buzz_stop failed: ") + e.what());
        throw;
    }
}

} // namespace device_reminder

