#pragma once
#include "infra/thread_message_operation/i_thread_message.hpp"

namespace device_reminder {

class IProcessMessage {
public:
    virtual ~IProcessMessage() = default;
    virtual MessageType type() const noexcept = 0;
    virtual bool payload() const noexcept = 0;
};

} // namespace device_reminder
