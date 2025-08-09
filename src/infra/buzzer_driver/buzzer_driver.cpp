#include "buzzer_driver/buzzer_driver.hpp"

#include <stdexcept>
#include <string>

namespace device_reminder {

BuzzerDriver::BuzzerDriver(std::shared_ptr<IFileLoader> loader,
                           std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IGPIOSetter> gpio)
    : logger_(std::move(logger)), gpio_(std::move(gpio)) {
    if (!logger_) { throw std::invalid_argument("ILogger is null"); }
    if (!loader) {
        logger_->error("IFileLoader is null");
        throw std::invalid_argument("IFileLoader is null");
    }
    if (!gpio_) {
        logger_->error("IGPIOSetter is null");
        throw std::invalid_argument("IGPIOSetter is null");
    }

    try {
        pin_ = loader->load_int();
    } catch (const std::exception& e) {
        logger_->error(std::string("Failed to load pin: ") + e.what());
        throw;
    }
    if (pin_ < 0) {
        logger_->error("Invalid pin number: " + std::to_string(pin_));
        throw std::runtime_error("Invalid pin number");
    }

    std::string active_level = "high";
    try {
        active_level = loader->load_string();
    } catch (...) {
        // use default
    }
    if (active_level == "high") {
        active_high_ = true;
    } else if (active_level == "low") {
        active_high_ = false;
    } else {
        logger_->error("Invalid active_level: " + active_level);
        throw std::runtime_error("Invalid active_level");
    }

    std::string initial_state = "off";
    try {
        initial_state = loader->load_string();
    } catch (...) {
        // use default
    }
    if (initial_state == "on") {
        is_on_ = true;
    } else if (initial_state == "off") {
        is_on_ = false;
    } else {
        logger_->error("Invalid initial_state: " + initial_state);
        throw std::runtime_error("Invalid initial_state");
    }

    try {
        gpio_->write(is_on_ ? active_high_ : !active_high_);
    } catch (const std::exception& e) {
        logger_->error(std::string("Failed to set initial state: ") + e.what());
        throw;
    }

    logger_->info("BuzzerDriver init: pin=" + std::to_string(pin_)
                  + " active_level=" + (active_high_ ? "high" : "low")
                  + " initial_state=" + (is_on_ ? "on" : "off"));
}

void BuzzerDriver::on() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_on_) {
        logger_->info("Buzzer already ON");
        return;
    }
    try {
        gpio_->write(active_high_);
    } catch (const std::exception& e) {
        logger_->error("Failed to turn ON buzzer pin " + std::to_string(pin_)
                        + ": " + e.what());
        throw;
    }
    is_on_ = true;
    logger_->info("Buzzer ON pin=" + std::to_string(pin_));
}

void BuzzerDriver::off() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!is_on_) {
        logger_->info("Buzzer already OFF");
        return;
    }
    try {
        gpio_->write(!active_high_);
    } catch (const std::exception& e) {
        logger_->error("Failed to turn OFF buzzer pin " + std::to_string(pin_)
                        + ": " + e.what());
        throw;
    }
    is_on_ = false;
    logger_->info("Buzzer OFF pin=" + std::to_string(pin_));
}

} // namespace device_reminder

