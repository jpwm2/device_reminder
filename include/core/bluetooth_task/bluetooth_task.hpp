#pragma once

#include "message/i_message.hpp"
#include "bluetooth_task/i_bluetooth_task.hpp"

namespace device_reminder {

class BluetoothTask : public IBluetoothTask {
public:
    BluetoothTask();
    ~BluetoothTask();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
