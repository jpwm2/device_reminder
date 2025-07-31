#pragma once

#include "interfaces/i_handler.hpp"
#include "infra/logger/i_logger.hpp"
#include "human_task/i_human_task.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"

#include <memory>

namespace device_reminder {

class HumanHandler : public IHandler {
public:
    HumanHandler(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IHumanTask> task,
                 std::shared_ptr<ITimerService> timer);

    void handle(std::shared_ptr<IProcessMessage> msg) override;

private:
    enum class State { Detecting, Stopping, Cooldown };

    std::shared_ptr<ILogger>       logger_;
    std::shared_ptr<IHumanTask>    task_;
    std::shared_ptr<ITimerService> timer_;
    State                          state_{State::Detecting};
};

} // namespace device_reminder
