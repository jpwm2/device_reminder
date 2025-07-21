#pragma once
#include "infra/message_type.hpp"

namespace device_reminder {

class IMessage {
public:
    virtual ~IMessage() = default;
    virtual MessageType type() const noexcept = 0;
    virtual bool payload() const noexcept = 0;
};

} // namespace device_reminder
