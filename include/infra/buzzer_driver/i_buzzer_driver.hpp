// buzzer_driver.hpp
#pragma once

namespace device_reminder {

class IBuzzerDriver {
public:
    virtual ~IBuzzerDriver() = default;
    virtual void start_buzzer() = 0;
    virtual void stop_buzzer() = 0;
};

} // namespace device_reminder
