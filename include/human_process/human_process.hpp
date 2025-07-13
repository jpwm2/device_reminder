#pragma once

#include "message/i_message.hpp"
#include "human_process/i_human_process.hpp"

namespace device_reminder {

class HumanProcess : public IHumanProcess {
public:
    HumanProcess();
    ~HumanProcess();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
