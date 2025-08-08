#pragma once

#include <string>
#include <vector>

namespace device_reminder {

struct BluetoothDevice {
    std::string mac;
    int distance;
};

class IBluetoothScanner {
public:
    virtual ~IBluetoothScanner() = default;
    virtual std::vector<BluetoothDevice> scan() = 0;
};

} // namespace device_reminder
