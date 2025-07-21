#include "thread_message_operation/thread_message_receiver.hpp"
#include "infra/logger/i_logger.hpp"
#include <utility>

namespace device_reminder {

ThreadMessageReceiver::ThreadMessageReceiver(std::shared_ptr<IThreadMessageQueue> queue,
                                             MessageHandler handler,
                                             std::shared_ptr<ILogger> logger)
    : queue_(std::move(queue)),
      handler_(std::move(handler)),
      logger_(std::move(logger)) {
    if (logger_) logger_->info("ThreadMessageReceiver created");
}

ThreadMessageReceiver::~ThreadMessageReceiver() {
    stop();
    if (logger_) logger_->info("ThreadMessageReceiver destroyed");
}

void ThreadMessageReceiver::stop() {
    running_ = false;
    if (queue_) queue_->close();
    if (logger_) logger_->info("ThreadMessageReceiver stop requested");
}

void ThreadMessageReceiver::operator()() {
    while (running_) {
        if (!queue_) break;
        ThreadMessage msg{};
        if (!queue_->pop(msg)) break;
        if (!running_) break;
        if (handler_) handler_(msg);
    }
    if (logger_) logger_->info("ThreadMessageReceiver loop end");
}

} // namespace device_reminder
