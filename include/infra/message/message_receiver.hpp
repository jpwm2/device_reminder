#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/message/message_dispatcher.hpp"
#include "infra/message/message_queue.hpp"

#include <atomic>
#include <memory>
#include <thread>

namespace device_reminder {

class IMessageReceiver {
public:
    virtual ~IMessageReceiver() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};

class MessageReceiver : public IMessageReceiver {
public:
    MessageReceiver(std::shared_ptr<ILogger> logger,
                    std::shared_ptr<IMessageQueue> queue,
                    std::shared_ptr<IMessageDispatcher> dispatcher);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IMessageQueue> queue_;
    std::shared_ptr<IMessageDispatcher> dispatcher_;
    std::atomic<bool> running_{false};
    std::thread thread_;
};

} // namespace device_reminder
