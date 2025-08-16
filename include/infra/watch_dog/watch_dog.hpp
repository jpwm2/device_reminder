// Copyright 2024 device_reminder
// WatchDog interface and implementation

#pragma once

#include <memory>

namespace device_reminder {

class ITimerService;
class IMessageQueue;
class IMessage;
class ILogger;
class IThreadSender;

class IWatchDog {
public:
    virtual ~IWatchDog() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void kick() = 0;
};

class WatchDog : public IWatchDog {
public:
    WatchDog(std::shared_ptr<ITimerService> timer_service,
             std::shared_ptr<IMessageQueue> message_queue,
             std::shared_ptr<IMessage> message,
             std::shared_ptr<IThreadSender> thread_sender,
             std::shared_ptr<ILogger> logger,
             int duration_ms = 0);

    void start() override;
    void stop() override;
    void kick() override;

private:
    std::shared_ptr<ITimerService> timer_service_{};
    std::shared_ptr<IMessageQueue> message_queue_{};
    std::shared_ptr<IMessage> message_{};
    std::shared_ptr<IThreadSender> thread_sender_{};
    std::shared_ptr<ILogger> logger_{};
    int duration_ms_{};
};

} // namespace device_reminder

