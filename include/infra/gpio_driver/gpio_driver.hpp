#pragma once

#include "gpio_driver/i_gpio_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include <string>

struct gpiod_chip;
struct gpiod_line;

namespace device_reminder {

class GPIODriver : public IGPIODriver {
public:
    GPIODriver(int pinNumber, Direction direction, ILogger* logger = nullptr,
               const std::string& chipName = "/dev/gpiochip0");
    ~GPIODriver() override;

    void write(bool value) override;
    bool read() override;
    void setEdge(EdgeType edge) override;
    void waitForEdge() override;

private:
    gpiod_chip* chip_;
    gpiod_line* line_;
    ILogger* logger_;
    Direction direction_;
    int pinNumber_;
    std::string chipName_;
};

} // namespace device_reminder
