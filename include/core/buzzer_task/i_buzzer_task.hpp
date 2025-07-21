#pragma once

#include "infra/i_message.hpp"

namespace device_reminder {

class IBuzzerTask {
public:
    virtual ~IBuzzerTask() = default;

    virtual void run() = 0;
    virtual bool send_message(const IMessage& msg) = 0;
};

} // namespace device_reminder
