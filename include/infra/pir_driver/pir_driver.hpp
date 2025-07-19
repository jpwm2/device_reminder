#pragma once

#include "pir_driver/i_pir_driver.hpp"
#include "gpio_driver/i_gpio_driver.hpp"
#include "infra/logger/logger.hpp" // ロガーのインターフェース（ILogger）を含むと仮定

#include <string>

namespace device_reminder {

class PIRDriver : public IPIRDriver {
public:
    PIRDriver(IGPIODriver* gpio_driver, ILogger* logger,
              unsigned int gpioLine = 17, const std::string& chipName = "/dev/gpiochip0");

    ~PIRDriver() override;

    int read() override;

private:
    IGPIODriver* gpio_;
    ILogger* logger_;
};

} // namespace device_reminder
