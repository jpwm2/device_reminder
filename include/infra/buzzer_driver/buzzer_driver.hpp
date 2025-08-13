#pragma once

#include "infra/file_loader.hpp"
#include "infra/gpio_operation/gpio_setter.hpp"
#include "infra/logger.hpp"

#include <memory>
#include <string>

namespace device_reminder {

class IBuzzerDriver {
public:
    virtual ~IBuzzerDriver() = default;

    virtual void on()  = 0;
    virtual void off() = 0;
};

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
