#pragma once

#include "message/i_message.hpp"
#include "human_task/i_human_task.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class HumanTask : public IHumanTask {
public:
    explicit HumanTask(std::shared_ptr<ILogger> logger);
    ~HumanTask();

    void run() override;
    bool send_message(const IMessage& msg) override;

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
