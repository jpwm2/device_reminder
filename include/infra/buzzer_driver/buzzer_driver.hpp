#pragma once
#include "i_buzzer_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/gpio_driver/i_gpio_driver.hpp"
#include <memory>
#include <string>

namespace device_reminder {

class BuzzerDriver : public IBuzzerDriver {
public:
    BuzzerDriver(IGPIODriver* gpio,
                 ILogger* logger = nullptr,
                 std::string chip = "/dev/gpiochip0",
                 unsigned int line = 18,
                 double freq_hz = 261.63,
                 double duty = 0.5,
                 std::string sysfs_base = "/sys/class/pwm");
    ~BuzzerDriver() override;

    bool start() override;
    bool stop() override;

private:
    bool exportPwm();
    bool unexportPwm();
    bool setFrequency(double hz);
    bool setDutyCycle(double ratio);
    bool enable(bool on);

    std::string chipPath_;
    std::string sysfsBase_;
    unsigned int line_;
    double freq_;
    double duty_;
    bool isOn_{false};
    int pwmChip_;
    int pwmChannel_;
    unsigned int periodNs_{0};
    IGPIODriver* gpio_;
    ILogger* logger_;
};

} // namespace device_reminder
