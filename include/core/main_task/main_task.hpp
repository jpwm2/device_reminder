#pragma once
#include "message/i_message.hpp"
#include "main_task/i_main_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "message_operator/i_message_sender.hpp"
#include <memory>

namespace device_reminder {

class MainTask : public IMainTask {
public:
    enum class State { WaitHumanDetect, WaitDeviceResponse, ScanCooldown };
    enum class TimerId { T_COOLDOWN, T_DET_TIMEOUT };

    MainTask(std::shared_ptr<ITimerService> det_timer,
             std::shared_ptr<ITimerService> cooldown_timer,
             std::shared_ptr<IMessageSender> human_sender,
             std::shared_ptr<IMessageSender> bluetooth_sender,
             std::shared_ptr<IMessageSender> buzzer_sender,
             std::shared_ptr<ILogger> logger);

    ~MainTask();

    void run(const IMessage& msg) override;
    State state() const noexcept { return state_; }

private:
    std::shared_ptr<ITimerService> det_timer_;
    std::shared_ptr<ITimerService> cooldown_timer_;
    std::shared_ptr<IMessageSender> human_sender_;
    std::shared_ptr<IMessageSender> bluetooth_sender_;
    std::shared_ptr<IMessageSender> buzzer_sender_;
    std::shared_ptr<ILogger> logger_;
    State state_{State::WaitHumanDetect};
};

} // namespace device_reminder
