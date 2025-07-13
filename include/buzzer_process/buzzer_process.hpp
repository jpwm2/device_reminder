#pragma once

#include "message/i_message.hpp"
#include "buzzer_process/i_buzzer_process.hpp"

namespace device_reminder {

class BuzzerProcess : public IBuzzerProcess {
public:
    BuzzerProcess();
    ~BuzzerProcess();

    void run() override;
    bool send_message(const IMessage& msg) override;
};

} // namespace device_reminder
