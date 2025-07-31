#pragma once
#include <memory>
#include <string>
#include <vector>
#include "infra/thread_operation/thread_message/thread_message_type.hpp"

namespace device_reminder {

class IThreadMessage {
public:
    virtual ~IThreadMessage() = default;
    virtual ThreadMessageType type() const noexcept = 0;
    virtual std::vector<std::string> payload() const = 0;
    virtual std::shared_ptr<IThreadMessage> clone() const = 0;
    virtual std::string to_string() const = 0;
};

} // namespace device_reminder
