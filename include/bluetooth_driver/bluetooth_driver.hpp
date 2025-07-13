// buzzer_driver.hpp
#pragma once
#include <string>

namespace device_reminder {

class BuzzerDriver {
public:
    virtual ~BuzzerDriver() = default;
    virtual void start_buzzer() = 0;
    virtual void stop_buzzer() = 0;
}

} // namespace device_reminder
