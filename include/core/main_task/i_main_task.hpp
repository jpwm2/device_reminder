#pragma once

#include "infra/thread_message_operation/i_thread_message.hpp"

namespace device_reminder {

class IMainTask {
public:
    virtual ~IMainTask() = default;

    virtual void run(const IThreadMessage& msg) = 0;
};

} // namespace device_reminder
