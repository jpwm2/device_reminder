// i_buzzer_driver.hpp
#pragma once

namespace device_reminder {

class IBuzzerDriver {
public:
    virtual ~IBuzzerDriver() = default;
    virtual void on()  = 0;   // ブザーON
    virtual void off() = 0;   // ブザーOFF
};

} // namespace device_reminder
