#pragma once
#include <memory>
#include "infra/process_operation/process_message/i_process_message.hpp"

namespace device_reminder {

class IProcessQueue {
public:
    virtual ~IProcessQueue() = default;
    virtual void push(std::shared_ptr<IProcessMessage> msg) = 0;
    virtual std::shared_ptr<IProcessMessage> pop() = 0;
    virtual std::size_t size() const = 0;
};

} // namespace device_reminder
