#pragma once

#include "infra/thread_operation/thread_message/i_thread_message.hpp"

namespace device_reminder {

class IBuzzerTask {
public:
    virtual ~IBuzzerTask() = default;

    virtual void run() = 0;
    virtual bool send_message(const IThreadMessage& msg) = 0;

    virtual void on_waiting(const std::vector<std::string>& payload) = 0;
    virtual void on_buzzing(const std::vector<std::string>& payload) = 0;
};

} // namespace device_reminder
