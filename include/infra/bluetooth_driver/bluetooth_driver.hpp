#pragma once

#include "i_bluetooth_driver.hpp"
#include "infra/logger/i_logger.hpp"

#include <vector>
#include <string>

namespace device_reminder {

class BluetoothDriver : public IBluetoothDriver {
public:
    explicit BluetoothDriver(ILogger& logger);

    std::vector<std::string> scan() override;

private:
    ILogger* logger_;
};

} // namespace device_reminder
