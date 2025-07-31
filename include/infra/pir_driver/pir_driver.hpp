#pragma once

#include "pir_driver/i_pir_driver.hpp"
#include "infra/gpio_operation/gpio_reader/i_gpio_reader.hpp"
#include "infra/logger/logger.hpp" // ロガーのインターフェース（ILogger）を含むと仮定

#include <string>

namespace device_reminder {

class PIRDriver : public IPIRDriver {
public:
    PIRDriver(IGPIOReader* gpio_driver, ILogger* logger);

    ~PIRDriver() override;

    int read() override;

private:
    IGPIOReader* gpio_;
    ILogger* logger_;
};

} // namespace device_reminder
