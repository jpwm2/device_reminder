#pragma once

#include "message/i_message.hpp"

namespace device_reminder {

class IBuzzerProcess {
public:
    virtual ~IBuzzerProcess() = default;

    virtual void run() = 0;
    virtual bool send_message(const IMessage& msg) = 0;
};

} // namespace device_reminder
