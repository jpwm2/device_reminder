#pragma once
#include "infra/thread_message_operation/i_message_queue.hpp"
#include <memory>

namespace device_reminder {

class IProcessMessageReceiver {
public:
    virtual ~IProcessMessageReceiver() = default;
    virtual void operator()() = 0;
    virtual void stop() = 0;
    virtual bool running() const noexcept = 0;
};

} // namespace device_reminder
