#pragma once

#include "infra/thread_message_operation/i_thread_message.hpp"

namespace device_reminder {

class IBluetoothTask {
public:
    virtual ~IBluetoothTask() = default;

    virtual void run(const IThreadMessage& msg) = 0;
    virtual bool send_message(const IThreadMessage& msg) = 0;
};

} // namespace device_reminder
