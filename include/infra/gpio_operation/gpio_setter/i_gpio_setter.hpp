#pragma once

namespace device_reminder {

class IGPIOSetter {
public:
    virtual ~IGPIOSetter() = default;
    virtual void write(bool value) = 0;
};

} // namespace device_reminder
