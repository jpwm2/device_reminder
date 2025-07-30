#pragma once
#include "infra/process_operation/process_message/process_message_type.hpp"
#include <memory>
#include <string>
#include <vector>

namespace device_reminder {

class IProcessMessage {
public:
    virtual ~IProcessMessage() = default;
    virtual ProcessMessageType type() const noexcept = 0;
    virtual std::vector<std::string> payload() const noexcept = 0;
    virtual std::shared_ptr<IProcessMessage> clone() const = 0;
    virtual std::string to_string() const = 0;
};

} // namespace device_reminder
