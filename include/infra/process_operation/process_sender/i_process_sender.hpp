#pragma once
#include "infra/process_message_operation/process_message.hpp"

namespace device_reminder {

class IProcessMessageSender {
public:
    virtual ~IProcessMessageSender() = default;
    virtual bool enqueue(const ProcessMessage& msg) = 0;
    virtual void stop() = 0;
};

} // namespace device_reminder
