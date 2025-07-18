#pragma once

#include "message/i_message.hpp"
#include "human_task/i_human_task.hpp"

namespace device_reminder {

class HumanTask : public IHumanTask {
public:
    HumanTask();
    ~HumanTask();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
