#pragma once

#include "core/interfaces/i_handler.hpp"
#include "infra/logger/i_logger.hpp"
#include "core/main_task/i_main_task.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/file_loader/i_file_loader.hpp"

#include <memory>

namespace device_reminder {

class MainHandler : public IHandler {
public:
    MainHandler(std::shared_ptr<ILogger> logger,
                std::shared_ptr<IMainTask> task,
                std::shared_ptr<ITimerService> timer_service,
                std::shared_ptr<IFileLoader> file_loader);

    void handle(std::shared_ptr<IProcessMessage> msg) override;

private:
    std::shared_ptr<ILogger>       logger_;
    std::shared_ptr<IMainTask>     task_;
    std::shared_ptr<ITimerService> timer_service_;
    std::shared_ptr<IFileLoader>   file_loader_;
};

} // namespace device_reminder
