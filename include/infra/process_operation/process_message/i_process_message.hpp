#pragma once
#include "infra/thread_message_operation/i_thread_message.hpp"

namespace device_reminder {

class IProcessMessage {
public:
    virtual ~IProcessMessage() = default;
    virtual ThreadMessageType type() const noexcept = 0;
    virtual bool payload() const noexcept = 0;
    virtual std::shared_ptr<IProcessMessage> clone() const = 0;
    virtual std::string to_string() const = 0;
};

} // namespace device_reminder
