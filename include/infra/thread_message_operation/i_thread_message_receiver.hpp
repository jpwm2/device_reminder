#pragma once
#include <functional>
#include "infra/thread_message_operation/thread_message.hpp"

namespace device_reminder {

class IThreadMessageReceiver {
public:
    using MessageHandler = std::function<void(const ThreadMessage&)>;
    virtual ~IThreadMessageReceiver() = default;
    virtual void operator()() = 0;
    virtual void stop() = 0;
    virtual bool running() const noexcept = 0;
};

} // namespace device_reminder
