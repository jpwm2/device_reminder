#pragma once
#include "message/i_message.hpp"
#include "main_process/i_main_process.hpp"

namespace device_reminder {

class MainProcess : public IMainProcess {
public:
    MainProcess();
    ~MainProcess();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
