#pragma once

#include "message/i_message.hpp"
#include "buzzer_task/i_buzzer_task.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class BuzzerTask : public IBuzzerTask {
public:
    explicit BuzzerTask(std::shared_ptr<ILogger> logger);
    ~BuzzerTask();

    void run() override;
    bool send_message(const IMessage& msg) override;

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
