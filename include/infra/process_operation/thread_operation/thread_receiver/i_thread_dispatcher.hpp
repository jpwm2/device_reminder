#pragma once
#include "infra/thread_message_operation/thread_message.hpp"

namespace device_reminder {
class IThreadDispatcher {
public:
    virtual ~IThreadDispatcher() = default;
    virtual void dispatch(const ThreadMessage& msg) = 0;
};
}
