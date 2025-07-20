#pragma once

#include "infra/process/process_base.hpp"
#include "buzzer_task/buzzer_handler.hpp"
#include <memory>

namespace device_reminder {

class BuzzerProcess : public ProcessBase {
public:
    BuzzerProcess(const std::string& mq_name,
                  std::shared_ptr<BuzzerTask> task,
                  std::shared_ptr<ILogger> logger)
        : ProcessBase(mq_name,
                      std::make_shared<BuzzerHandler>(std::move(task)),
                      std::move(logger),
                      2) {}
};

} // namespace device_reminder
