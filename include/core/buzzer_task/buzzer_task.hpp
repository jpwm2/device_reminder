#pragma once

#include "message/i_message.hpp"
#include "buzzer_task/i_buzzer_task.hpp"

namespace device_reminder {

class BuzzerTask : public IBuzzerTask {
public:
    BuzzerTask();
    ~BuzzerTask();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
