#pragma once
#include "timer_service/i_timer_service.hpp"
#include "message_operator/i_message_sender.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>

namespace device_reminder {

class TimerService : public ITimerService {
public:
    explicit TimerService(std::shared_ptr<IMessageSender> sender);
    ~TimerService() override;

    void start(uint32_t milliseconds,
               uint32_t timeout_msg) override;
    void stop() override;
    bool active() const noexcept override { return active_; }

private:
    void worker(uint32_t milliseconds,
                uint32_t timeout_msg);

    std::shared_ptr<IMessageSender> sender_;
    std::thread                     thread_;
    std::atomic<bool>               active_{false};
    std::atomic<bool>               running_{false};
};

} // namespace device_reminder
