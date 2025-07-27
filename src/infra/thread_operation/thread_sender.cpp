#include "thread_operation/thread_sender.hpp"
#include "infra/logger/i_logger.hpp"
#include <utility>

namespace device_reminder {

ThreadSender::ThreadSender(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IThreadQueue<std::shared_ptr<IThreadMessage>>> queue,
                           std::shared_ptr<IThreadMessage> message)
    : logger_(std::move(logger)),
      queue_(std::move(queue)),
      message_(std::move(message)) {
    if (logger_) logger_->info("ThreadSender created");
}

void ThreadSender::send() {
    if (!queue_ || !message_) {
        if (logger_) logger_->error("ThreadSender send failed: null queue or message");
        return;
    }
    queue_->push(message_);
}

} // namespace device_reminder
