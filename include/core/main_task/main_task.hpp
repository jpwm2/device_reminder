#pragma once
#include "infra/thread_message_operation/i_thread_message.hpp"
#include "main_task/i_main_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "process_message_operation/i_process_message_sender.hpp"
#include <memory>

namespace device_reminder {

class MainTask : public IMainTask {
public:
    enum class State { WaitHumanDetect, WaitDeviceResponse, ScanCooldown };
    enum class TimerId { T_COOLDOWN, T_DET_TIMEOUT };

    MainTask(std::shared_ptr<ITimerService> det_timer,
             std::shared_ptr<ITimerService> cooldown_timer,
             std::shared_ptr<IProcessMessageSender> human_sender,
             std::shared_ptr<IProcessMessageSender> bluetooth_sender,
             std::shared_ptr<IProcessMessageSender> buzzer_sender,
             std::shared_ptr<ILogger> logger);

    ~MainTask();

    void run(const IThreadMessage& msg) override;
    State state() const noexcept { return state_; }

private:
    std::shared_ptr<ITimerService> det_timer_;
    std::shared_ptr<ITimerService> cooldown_timer_;
    std::shared_ptr<IProcessMessageSender> human_sender_;
    std::shared_ptr<IProcessMessageSender> bluetooth_sender_;
    std::shared_ptr<IProcessMessageSender> buzzer_sender_;
    std::shared_ptr<ILogger> logger_;
    State state_{State::WaitHumanDetect};
};

} // namespace device_reminder
