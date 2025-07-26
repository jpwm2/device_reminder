#include "thread_message_operation/thread_receiver.hpp"
#include "infra/logger/i_logger.hpp"
#include <utility>

namespace device_reminder {

ThreadReceiver::ThreadReceiver(std::shared_ptr<IThreadMessageQueue> queue,
                               std::shared_ptr<IThreadDispatcher> dispatcher,
                               std::shared_ptr<ILogger> logger)
    : queue_(std::move(queue)),
      dispatcher_(std::move(dispatcher)),
      logger_(std::move(logger)) {
    if (logger_) logger_->info("ThreadReceiver created");
}

void ThreadReceiver::stop() {
    running_ = false;
    if (queue_) queue_->close();
    if (logger_) logger_->info("ThreadReceiver stop requested");
}

void ThreadReceiver::run() {
    while (running_) {
        if (!queue_) break;
        ThreadMessage msg{};
        if (!queue_->pop(msg)) break;
        if (!running_) break;
        if (dispatcher_) dispatcher_->dispatch(msg);
    }
    if (logger_) logger_->info("ThreadReceiver loop end");
}

} // namespace device_reminder
