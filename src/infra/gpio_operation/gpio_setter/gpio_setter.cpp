#include "gpio_operation/gpio_setter/gpio_setter.hpp"
#include <gpiod.h>
#include <stdexcept>

namespace device_reminder {

GPIOSetter::GPIOSetter(std::shared_ptr<ILogger> logger,
                       int pin_no,
                       std::shared_ptr<IFileLoader> loader)
    : logger_(std::move(logger)), pin_no_(pin_no), loader_(std::move(loader)) {}

void GPIOSetter::write(bool is_high) {
    if (logger_) {
        logger_->info(std::string("GPIOSetter write ") + (is_high ? "HIGH" : "LOW"));
    }

    if (!loader_) {
        if (logger_) logger_->error("FileLoader is null");
        throw std::runtime_error("FileLoader is null");
    }

    std::string chip_name;
    try {
        chip_name = loader_->load_string();
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("Failed to load GPIO chip name: ") + e.what());
        throw;
    }

    gpiod_chip* chip = gpiod_chip_open(chip_name.c_str());
    if (!chip) {
        if (logger_) logger_->error("Failed to open GPIO chip: " + chip_name);
        throw std::runtime_error("Failed to open GPIO chip: " + chip_name);
    }

    gpiod_line* line = gpiod_chip_get_line(chip, static_cast<unsigned int>(pin_no_));
    if (!line) {
        if (logger_) logger_->error("Failed to get GPIO line: " + std::to_string(pin_no_));
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to get GPIO line");
    }

    if (gpiod_line_request_output(line, "device_reminder", 0) < 0) {
        if (logger_) logger_->error("Failed to request GPIO line");
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to request GPIO line");
    }

    if (gpiod_line_set_value(line, is_high ? 1 : 0) < 0) {
        if (logger_) logger_->error("Failed to write GPIO line value");
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        throw std::runtime_error("Failed to write GPIO line value");
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);

    if (logger_) {
        logger_->info("GPIOSetter write success");
    }
}

} // namespace device_reminder

