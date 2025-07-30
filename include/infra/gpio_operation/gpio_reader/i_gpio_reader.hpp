#pragma once

namespace device_reminder {

class IGPIOReader {
public:
    virtual ~IGPIOReader() = default;
    virtual bool read() = 0; // true: HIGH, false: LOW
};

} // namespace device_reminder
