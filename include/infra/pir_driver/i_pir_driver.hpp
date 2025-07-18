#pragma once

namespace device_reminder {

class IPIRDriver {
public:
    virtual ~IPIRDriver() = default;

    // PIRセンサーの値を読み取る（0または1）
    virtual int read() = 0;
};

} // namespace device_reminder
