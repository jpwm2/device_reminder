#pragma once

#include "human_task/i_human_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/pir_driver/i_pir_driver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include <memory>

namespace device_reminder {

class HumanTask : public IHumanTask {
public:
    HumanTask(std::shared_ptr<ILogger> logger,
              std::shared_ptr<IPIRDriver> pir,
              std::shared_ptr<IProcessSender> sender);

    void on_detecting(const std::vector<std::string>& payload) override;
    void on_stopping(const std::vector<std::string>& payload) override;
    void on_cooldown(const std::vector<std::string>& payload) override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IPIRDriver> pir_;
    std::shared_ptr<IProcessSender> sender_;
};

} // namespace device_reminder
