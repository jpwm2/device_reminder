#pragma once

#include "gpio_driver/i_gpio_driver.hpp"
#include <string>

struct gpiod_chip;
struct gpiod_line;

namespace device_reminder {

class GPIODriver : public IGPIODriver {
public:
    GPIODriver();
    ~GPIODriver() override;

    void openChip(const std::string& chipName) override;
    void setupLine(unsigned int lineNumber) override;
    int readLine() override;
    void close() override;

private:
    gpiod_chip* chip_;
    gpiod_line* line_;
};

} // namespace device_reminder
