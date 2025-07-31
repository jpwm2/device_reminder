#pragma once

#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include <vector>
#include <string>

namespace device_reminder {

class IMainTask {
public:
    virtual ~IMainTask() = default;

    virtual void run(const IThreadMessage& msg) = 0;
    virtual void on_waiting_for_human(const std::vector<std::string>& payload) = 0;
    virtual void on_response_to_buzzer_task(const std::vector<std::string>& payload) = 0;
    virtual void on_response_to_human_task(const std::vector<std::string>& payload) = 0;
    virtual void on_cooldown(const std::vector<std::string>& payload) = 0;
    virtual void on_waiting_for_second_response(const std::vector<std::string>& payload) = 0;
};

} // namespace device_reminder
