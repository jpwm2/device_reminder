#pragma once

#include "infra/message/message_receiver.hpp"
#include "infra/message/message_inbox.hpp"
#include "infra/logger.hpp"

#include <atomic>
#include <memory>
#include <sys/types.h>

namespace device_reminder {

class IProcess {
public:
    virtual ~IProcess() = default;
    virtual pid_t spawn() = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
};

class Process : public IProcess {
public:
    Process(std::shared_ptr<IMessageReceiver> receiver,
            std::shared_ptr<ILogger> logger,
            std::shared_ptr<MessageInbox> inbox_dep,
            MessageInbox inbox);

    pid_t spawn() override;
    void run() override;
    void stop() override;

private:
    std::shared_ptr<IMessageReceiver> receiver_;
    std::shared_ptr<ILogger> logger_;
    [[maybe_unused]] std::shared_ptr<MessageInbox> inbox_dep_;
    [[maybe_unused]] MessageInbox inbox_;
    std::atomic<bool> running_{false};
};

} // namespace device_reminder

