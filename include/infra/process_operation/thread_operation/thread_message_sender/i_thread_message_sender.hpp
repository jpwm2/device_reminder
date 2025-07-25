#pragma once
#include "infra/thread_message_operation/thread_message.hpp"

namespace device_reminder {

class IThreadMessageSender {
public:
    virtual ~IThreadMessageSender() = default;
    virtual bool enqueue(const ThreadMessage& msg) = 0;
    virtual void stop() = 0;
};

} // namespace device_reminder
