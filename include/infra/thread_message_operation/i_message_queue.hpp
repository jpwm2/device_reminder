#pragma once
#include <optional>
#include "infra/thread_message_operation/thread_message.hpp"

namespace device_reminder {

class IThreadMessageQueue {
public:
    IThreadMessageQueue() = default;
    virtual ~IThreadMessageQueue() = default;
    IThreadMessageQueue(const IThreadMessageQueue&) = delete;
    IThreadMessageQueue& operator=(const IThreadMessageQueue&) = delete;

    virtual bool push(const ThreadMessage& msg) = 0;
    virtual std::optional<ThreadMessage> pop() = 0;
    virtual bool pop(ThreadMessage& out) = 0;
    virtual bool is_open() const noexcept = 0;
    virtual void close() = 0;
};

} // namespace device_reminder
