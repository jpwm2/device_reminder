#pragma once

#include <string>
#include <vector>

namespace device_reminder {

class IBluetoothTask {
public:
    virtual ~IBluetoothTask() = default;

    virtual void on_waiting(const std::vector<std::string>& payload) = 0;
};

} // namespace device_reminder
