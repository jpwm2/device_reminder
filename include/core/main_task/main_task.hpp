#pragma once
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include "main_task/i_main_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/timer_service/i_timer_service.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include <memory>

namespace device_reminder {

class MainTask : public IMainTask {
public:
    enum class State { WaitHumanDetect, WaitDeviceResponse, ScanCooldown };
    enum class TimerId { T_COOLDOWN, T_DET_TIMEOUT };

    MainTask(std::shared_ptr<ILogger> logger,
             std::shared_ptr<IFileLoader> file_loader,
             std::shared_ptr<IProcessSender> human_start_sender,
             std::shared_ptr<IProcessSender> human_stop_sender,
             std::shared_ptr<IProcessSender> bluetooth_sender,
             std::shared_ptr<IProcessSender> buzzer_start_sender,
             std::shared_ptr<IProcessSender> buzzer_stop_sender,
             std::shared_ptr<ITimerService> det_timer,
             std::shared_ptr<ITimerService> cooldown_timer);

    ~MainTask();

    void run(const IThreadMessage& msg) override;
    void on_waiting_for_human(const std::vector<std::string>& payload) override;
    void on_response_to_buzzer_task(const std::vector<std::string>& payload) override;
    void on_response_to_human_task(const std::vector<std::string>& payload) override;
    void on_cooldown(const std::vector<std::string>& payload) override;
    void on_waiting_for_second_response(const std::vector<std::string>& payload) override;
    State state() const noexcept { return state_; }

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IFileLoader> file_loader_;
    std::shared_ptr<IProcessSender> human_start_sender_;
    std::shared_ptr<IProcessSender> human_stop_sender_;
    std::shared_ptr<IProcessSender> bluetooth_sender_;
    std::shared_ptr<IProcessSender> buzzer_start_sender_;
    std::shared_ptr<IProcessSender> buzzer_stop_sender_;
    std::shared_ptr<ITimerService> det_timer_;
    std::shared_ptr<ITimerService> cooldown_timer_;
    State state_{State::WaitHumanDetect};
};

} // namespace device_reminder
