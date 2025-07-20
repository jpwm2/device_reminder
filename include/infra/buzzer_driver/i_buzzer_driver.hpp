// buzzer_driver.hpp
#pragma once

namespace device_reminder {

class IBuzzerDriver {
public:
    virtual ~IBuzzerDriver() = default;
    virtual bool start() = 0;   // true: 成功
    virtual bool stop()  = 0;   // true: 成功
}; 

} // namespace device_reminder
