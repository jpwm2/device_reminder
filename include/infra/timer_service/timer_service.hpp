#pragma once

#include "infra/logger.hpp"
#include "infra/message/thread_sender.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"

#include <atomic>
#include <future>
#include <memory>

namespace device_reminder {

class ITimerService {
public:
    virtual ~ITimerService() = default;
    virtual void start(int duration_ms,
                      std::shared_ptr<IThreadSender> sender,
                      std::shared_ptr<IMessageQueue> queue,
                      std::shared_ptr<IMessage> message) = 0;
    virtual void stop() = 0;
};

class TimerService : public ITimerService {
public:
    explicit TimerService(std::shared_ptr<ILogger> logger);
    ~TimerService() override;

    void start(int duration_ms,
               std::shared_ptr<IThreadSender> sender,
               std::shared_ptr<IMessageQueue> queue,
               std::shared_ptr<IMessage> message) override;
    void stop() override;

private:
    void worker(int duration_ms);

    std::shared_ptr<ILogger> logger_{};
    std::shared_ptr<IThreadSender> sender_{};
    std::shared_ptr<IMessageQueue> queue_{};
    std::shared_ptr<IMessage> message_{};
    std::future<void> worker_future_{};
    std::atomic<bool> cancel_{false};
    std::atomic<bool> running_{false};
};

} // namespace device_reminder
