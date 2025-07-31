#pragma once

namespace device_reminder {

class IBluetoothProcess {
public:
    virtual ~IBluetoothProcess() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};

} // namespace device_reminder
