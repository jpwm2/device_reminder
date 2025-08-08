#pragma once

#include "infra/gpio_operation/gpio_setter/i_gpio_setter.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/file_loader/i_file_loader.hpp"

#include <memory>

namespace device_reminder {

class GPIOSetter : public IGPIOSetter {
public:
    GPIOSetter(std::shared_ptr<ILogger> logger,
               int pin_no,
               std::shared_ptr<IFileLoader> loader);

    void write(bool is_high) override;

private:
    std::shared_ptr<ILogger> logger_;
    int pin_no_;
    std::shared_ptr<IFileLoader> loader_;
};

} // namespace device_reminder
