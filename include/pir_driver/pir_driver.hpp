#pragma once
#include <string>

namespace device_reminder {

class IGPIODriver;
class ILogger;

class PIRDriver {
public:
    PIRDriver(IGPIODriver* gpio_driver, ILogger* logger, unsigned int gpioLine = 17, const std::string& chipName = "/dev/gpiochip0");
    ~PIRDriver();
    int read();

private:
    IGPIODriver* gpio_;
    ILogger* logger_;
    void* chip_;
    void* line_;
};

} // namespace device_reminder