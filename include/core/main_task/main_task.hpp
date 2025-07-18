#pragma once
#include "message/i_message.hpp"
#include "main_task/i_main_task.hpp"

namespace device_reminder {

class MainTask : public IMainTask {
public:
    MainTask();
    ~MainTask();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
