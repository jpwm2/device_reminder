#include "infra/thread_operation/thread_receiver/thread_receiver.hpp"
#include "infra/logger/i_logger.hpp"
#include <utility>
#include <memory>

namespace device_reminder {

ThreadReceiver::ThreadReceiver(std::shared_ptr<IThreadQueue> queue,
                               std::shared_ptr<IThreadDispatcher> dispatcher,
                               std::shared_ptr<ILogger> logger)
    : queue_(std::move(queue)),
      dispatcher_(std::move(dispatcher)),
      logger_(std::move(logger)) {
    if (logger_) logger_->info("ThreadReceiver created");
}

void ThreadReceiver::stop() {
    running_ = false;
    if (logger_) logger_->info("ThreadReceiver stop requested");
}

void ThreadReceiver::run() {
    while (running_) {
        if (!queue_) break;
        auto msg = queue_->pop();
        if (!msg) break;
        if (!running_) break;
        if (dispatcher_) dispatcher_->dispatch(msg);
    }
    if (logger_) logger_->info("ThreadReceiver loop end");
}

} // namespace device_reminder
