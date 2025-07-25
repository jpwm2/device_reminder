#pragma once
#include "timer_service/i_timer_service.hpp"
#include "process_message_operation/i_process_message_sender.hpp"
#include "process_message_operation/process_message.hpp"
#include "infra/logger/i_logger.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>

namespace device_reminder {

class TimerService : public ITimerService {
public:
    explicit TimerService(std::shared_ptr<IProcessMessageSender> sender,
                          std::shared_ptr<ILogger> logger = nullptr);
    ~TimerService() override;

    void start(uint32_t milliseconds,
               const ProcessMessage& timeout_msg) override;
    void stop() override;
    bool active() const noexcept override { return active_; }

private:
    void worker(uint32_t milliseconds,
                ProcessMessage timeout_msg);

    std::shared_ptr<IProcessMessageSender> sender_;
    std::shared_ptr<ILogger>        logger_;
    std::thread                     thread_;
    std::atomic<bool>               active_{false};
    std::atomic<bool>               running_{false};
};

} // namespace device_reminder
