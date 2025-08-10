#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace device_reminder {

class IMessageQueue {
public:
    virtual ~IMessageQueue() = default;
    virtual void push(std::shared_ptr<IMessage> msg) = 0;
    virtual std::shared_ptr<IMessage> pop() = 0;
};

class MessageQueue : public IMessageQueue {
public:
    explicit MessageQueue(std::shared_ptr<ILogger> logger);

    void push(std::shared_ptr<IMessage> msg) override;
    std::shared_ptr<IMessage> pop() override;

private:
    std::shared_ptr<ILogger> logger_{};
    std::mutex mtx_{};
    std::condition_variable cv_{};
    std::queue<std::shared_ptr<IMessage>> queue_{};
};

} // namespace device_reminder

