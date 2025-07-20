#pragma once

#include "message/i_message.hpp"
#include "human_task/i_human_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/pir_driver/i_pir_driver.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "message_operator/i_message_sender.hpp"
#include <memory>

namespace device_reminder {

class HumanTask : public IHumanTask {
public:
    enum class State { Detecting, Stopped, Cooldown };

    HumanTask(std::shared_ptr<IPIRDriver> pir,
              std::shared_ptr<ITimerService> timer,
              std::shared_ptr<IMessageSender> sender,
              std::shared_ptr<ILogger> logger);
    ~HumanTask();

    void run(const IMessage& msg) override;

    State state() const noexcept { return state_; }

private:
    std::shared_ptr<IPIRDriver>   pir_;
    std::shared_ptr<ITimerService> timer_;
    std::shared_ptr<IMessageSender> sender_;
    std::shared_ptr<ILogger>        logger_;
    State state_{State::Detecting};
};

} // namespace device_reminder
