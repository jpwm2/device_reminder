#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"

#include <memory>

namespace device_reminder {

class IThreadSender {
public:
    virtual ~IThreadSender() = default;
    virtual void send(std::shared_ptr<IMessageQueue> queue,
                      std::shared_ptr<IMessage> message) = 0;
};

class ThreadSender : public IThreadSender {
public:
    explicit ThreadSender(std::shared_ptr<ILogger> logger);
    void send(std::shared_ptr<IMessageQueue> queue,
              std::shared_ptr<IMessage> message) override;

private:
    std::shared_ptr<ILogger> logger_{};
};

} // namespace device_reminder

