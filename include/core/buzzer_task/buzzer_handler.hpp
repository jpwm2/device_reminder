#pragma once

#include "core/interfaces/i_handler.hpp"
#include "infra/logger/i_logger.hpp"
#include "core/buzzer_task/i_buzzer_task.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"
#include <memory>

namespace device_reminder {

class BuzzerHandler : public IHandler {
public:
    BuzzerHandler(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IBuzzerTask> task,
                 std::shared_ptr<ITimerService> timer);
    void handle(std::shared_ptr<IProcessMessage> msg) override;

private:
    std::shared_ptr<ILogger>       logger_;
    std::shared_ptr<IBuzzerTask>   task_;
    std::shared_ptr<ITimerService> timer_;
};

} // namespace device_reminder
