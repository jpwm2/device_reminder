#pragma once

#include "infra/i_message.hpp"

namespace device_reminder {

class IBluetoothTask {
public:
    virtual ~IBluetoothTask() = default;

    virtual void run(const IMessage& msg) = 0;
    virtual bool send_message(const IMessage& msg) = 0;
};

} // namespace device_reminder
