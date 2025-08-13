#pragma once

#include "infra/logger.hpp"
#include "infra/message/thread_sender.hpp"

#include <atomic>
#include <exception>
#include <memory>
#include <thread>

namespace device_reminder {

class ITimerService {
public:
    virtual ~ITimerService() = default;
    virtual void start(int duration_ms, std::shared_ptr<IThreadSender> sender) = 0;
    virtual void stop() = 0;
};

class TimerService : public ITimerService {
public:
    explicit TimerService(std::shared_ptr<ILogger> logger);
    ~TimerService() override;

    void start(int duration_ms, std::shared_ptr<IThreadSender> sender) override;
    void stop() override;

private:
    void worker(int duration_ms);

    std::shared_ptr<ILogger> logger_{};
    std::shared_ptr<IThreadSender> sender_{};
    std::thread thread_{};
    std::atomic<bool> cancel_{false};
    std::atomic<bool> running_{false};
    std::exception_ptr worker_exception_{};
};

} // namespace device_reminder
