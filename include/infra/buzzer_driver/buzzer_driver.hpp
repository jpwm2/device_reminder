#pragma once
#include "i_buzzer_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/gpio_operation/gpio_setter/i_gpio_setter.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include <memory>
#include <string>

namespace device_reminder {

class BuzzerDriver : public IBuzzerDriver {
public:
    BuzzerDriver(std::shared_ptr<IFileLoader> loader,
                 std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IGPIOSetter> gpio);
    ~BuzzerDriver() override = default;

    void on() override;
    void off() override;

private:
    std::shared_ptr<IFileLoader> loader_;
    std::shared_ptr<ILogger>     logger_;
    std::shared_ptr<IGPIOSetter> gpio_;
}; 

} // namespace device_reminder
