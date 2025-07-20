#pragma once
#include "message/i_message.hpp"
#include "main_task/i_main_task.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class MainTask : public IMainTask {
public:
    explicit MainTask(std::shared_ptr<ILogger> logger);
    ~MainTask();

    void run() override;
    bool send_message(const IMessage& msg) override;

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
