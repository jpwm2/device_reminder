#pragma once
#include "infra/thread_message_operation/i_message_queue.hpp"
#include "infra/thread_message_operation/thread_message.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_message_operation/i_thread_message_sender.hpp"
#include <memory>

namespace device_reminder {

class ThreadMessageSender : public IThreadMessageSender {
public:
    explicit ThreadMessageSender(std::shared_ptr<IThreadMessageQueue> queue,
                                 std::shared_ptr<ILogger> logger = nullptr);
    bool enqueue(const ThreadMessage& msg);
    void stop();

private:
    std::shared_ptr<IThreadMessageQueue> queue_;
    std::shared_ptr<ILogger>             logger_;
};

} // namespace device_reminder
