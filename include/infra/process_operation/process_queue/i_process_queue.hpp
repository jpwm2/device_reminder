#pragma once
#include <memory>
#include "infra/message/i_message.hpp"

namespace device_reminder {

class IProcessQueue {
public:
    virtual ~IProcessQueue() = default;
    virtual void push(std::shared_ptr<IMessage> msg) = 0;
    virtual std::shared_ptr<IMessage> pop() = 0;
    virtual std::size_t size() const = 0;
};

} // namespace device_reminder
