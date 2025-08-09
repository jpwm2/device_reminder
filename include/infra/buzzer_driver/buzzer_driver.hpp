#pragma once
#include "i_buzzer_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/gpio_operation/gpio_setter/i_gpio_setter.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include <memory>
#include <mutex>
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
    int pin_{};
    bool active_high_{true};
    std::shared_ptr<ILogger>     logger_;
    std::shared_ptr<IGPIOSetter> gpio_;
    std::mutex mutex_;
    bool is_on_{false};
};

} // namespace device_reminder
