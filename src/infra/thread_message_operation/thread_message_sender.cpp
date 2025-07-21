#include "thread_message_operation/thread_message_sender.hpp"
#include "infra/logger/i_logger.hpp"
#include <utility>

namespace device_reminder {

ThreadMessageSender::ThreadMessageSender(std::shared_ptr<IThreadMessageQueue> queue,
                                         std::shared_ptr<ILogger> logger)
    : queue_(std::move(queue)),
      logger_(std::move(logger)) {
    if (logger_) logger_->info("ThreadMessageSender created");
}

bool ThreadMessageSender::enqueue(const ThreadMessage& msg) {
    if (!queue_) return false;
    if (logger_) logger_->info("ThreadMessageSender enqueue");
    return queue_->push(msg);
}

void ThreadMessageSender::stop() {
    if (queue_) queue_->close();
    if (logger_) logger_->info("ThreadMessageSender stop");
}

} // namespace device_reminder
