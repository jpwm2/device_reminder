#pragma once

#include "message/i_message.hpp"

namespace device_reminder {

class IMainTask {
public:
    virtual ~IMainTask() = default;

    virtual void run(const IMessage& msg) = 0;
};

} // namespace device_reminder
