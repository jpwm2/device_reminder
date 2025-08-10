#pragma once

#include "i_gpio_reader.hpp"
#include "infra/logger/i_logger.hpp"

#include <memory>
#include <string>

struct gpiod_chip;
struct gpiod_line;

namespace device_reminder {

class GPIOReader : public IGPIOReader {
public:
    GPIOReader(std::shared_ptr<ILogger> logger,
               int pin_no,
               std::string chip_name = "/dev/gpiochip0");
    ~GPIOReader() override;

    bool read() override;

private:
    std::shared_ptr<ILogger> logger_;
    int pin_no_;
    std::string chip_name_;
    gpiod_chip* chip_;
    gpiod_line* line_;
};

} // namespace device_reminder
