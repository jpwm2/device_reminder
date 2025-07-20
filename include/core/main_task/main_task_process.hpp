#pragma once

#include "infra/process/process_base.hpp"
#include "main_task/main_task_handler.hpp"
#include <memory>

namespace device_reminder {

class MainTaskProcess : public ProcessBase {
public:
    MainTaskProcess(const std::string& mq_name,
                    std::shared_ptr<MainTask> task,
                    std::shared_ptr<ILogger> logger)
        : ProcessBase(mq_name,
                      std::make_shared<MainTaskHandler>(std::move(task)),
                      std::move(logger),
                      3) {}
};

} // namespace device_reminder
