#pragma once

#include "message/i_message.hpp"
#include "bluetooth_process/i_bluetooth_process.hpp"

namespace device_reminder {

class BluetoothProcess : public IBluetoothProcess {
public:
    BluetoothProcess();
    ~BluetoothProcess();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
