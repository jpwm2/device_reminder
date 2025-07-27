#pragma once
#include <optional>
#include "infra/process_message_operation/process_message.hpp"

namespace device_reminder {

class IProcessMessageQueue {
public:
    virtual ~IProcessMessageQueue() = default;
    virtual bool push(const ProcessMessage& msg) = 0;
    virtual std::optional<ProcessMessage> pop() = 0;
    virtual bool pop(ProcessMessage& out) = 0;
    virtual bool is_open() const noexcept = 0;
    virtual void close() = 0;
};

} // namespace device_reminder
