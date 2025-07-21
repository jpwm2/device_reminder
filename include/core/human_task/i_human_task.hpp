#pragma once

#include "infra/i_message.hpp"

namespace device_reminder {

class IHumanTask {
public:
    virtual ~IHumanTask() = default;

    virtual void run(const IMessage& msg) = 0;
};

} // namespace device_reminder
