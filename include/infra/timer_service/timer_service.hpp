#pragma once

#include "infra/timer_service/i_timer_service.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"

#include <thread>
#include <atomic>
#include <memory>

namespace device_reminder {

class TimerService : public ITimerService {
public:
    explicit TimerService(std::shared_ptr<ILogger> logger,
                          int duration_ms,
                          std::shared_ptr<IThreadSender> sender);
    ~TimerService() override;

    void start() override;
    void stop() override;

private:
    void worker();

    std::shared_ptr<ILogger> logger_;
    int duration_ms_{0};
    std::shared_ptr<IThreadSender> sender_;
    std::thread thread_;
    std::atomic<bool> cancel_flag_{false};
};

} // namespace device_reminder
