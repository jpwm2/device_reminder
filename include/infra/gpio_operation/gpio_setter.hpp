#pragma once

#include "infra/gpio_operation/gpio_setter/i_gpio_setter.hpp"
#include "infra/logger/i_logger.hpp"

#include <memory>
#include <string>

struct gpiod_chip;
struct gpiod_line;

namespace device_reminder {

class GPIOSetter : public IGPIOSetter {
public:
    GPIOSetter(std::shared_ptr<ILogger> logger,
               int pin_no,
               std::string chip_name = "/dev/gpiochip0");
    ~GPIOSetter() override;

    void write(bool value) override;

private:
    std::shared_ptr<ILogger> logger_;
    gpiod_chip* chip_;
    gpiod_line* line_;
};

} // namespace device_reminder
