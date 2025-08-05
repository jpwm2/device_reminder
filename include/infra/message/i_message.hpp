#pragma once
#include "infra/message/message_type.hpp"
#include <string>
#include <vector>

namespace device_reminder {

using infra::MessageType;

class IMessage {
public:
    virtual ~IMessage() = default;

    virtual MessageType type() const noexcept = 0;
    virtual std::vector<std::string> payload() const noexcept = 0;
    virtual std::string to_string() const = 0;
};

} // namespace device_reminder

