#pragma once

#include "infra/process/process_base.hpp"
#include "human_task/human_task_handler.hpp"
#include <memory>

namespace device_reminder {

class HumanTaskProcess : public ProcessBase {
public:
    HumanTaskProcess(const std::string& mq_name,
                     std::shared_ptr<HumanTask> task,
                     std::shared_ptr<ILogger> logger)
        : ProcessBase(mq_name,
                      std::make_shared<HumanTaskHandler>(std::move(task)),
                      std::move(logger),
                      4) {}
};

} // namespace device_reminder
