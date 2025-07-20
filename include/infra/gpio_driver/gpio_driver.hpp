#pragma once

#include "gpio_driver/i_gpio_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include <string>
#include <memory>

struct gpiod_chip;
struct gpiod_line;

namespace device_reminder {

class GPIODriver : public IGPIODriver {
public:
    explicit GPIODriver(std::shared_ptr<ILogger> logger);
    ~GPIODriver() override;

    void openChip(const std::string& chipName) override;
    void setupLine(unsigned int lineNumber) override;
    int readLine() override;
    void close() override;

private:
    gpiod_chip* chip_;
    gpiod_line* line_;
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
