#pragma once

#include "infra/thread_message_operation/i_thread_message.hpp"

namespace device_reminder {

class IBuzzerTask {
public:
    virtual ~IBuzzerTask() = default;

    virtual void run() = 0;
    virtual bool send_message(const IThreadMessage& msg) = 0;
};

} // namespace device_reminder
