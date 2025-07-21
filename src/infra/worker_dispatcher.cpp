#include "worker_dispatcher/worker_dispatcher.hpp"
#include "thread_message_operation/thread_message.hpp"
#include "infra/logger/i_logger.hpp"

#include <utility>

namespace device_reminder {

WorkerDispatcher::WorkerDispatcher(std::shared_ptr<IThreadMessageQueue> queue,
                                   MessageHandler handler,
                                   std::shared_ptr<ILogger> logger)
    : queue_(std::move(queue)),
      handler_(std::move(handler)),
      logger_(std::move(logger)) {
    if (logger_) logger_->info("WorkerDispatcher created");
}

WorkerDispatcher::~WorkerDispatcher() {
    stop();
    join();
    if (logger_) logger_->info("WorkerDispatcher destroyed");
}

void WorkerDispatcher::start() {
    if (running_) return;
    running_ = true;
    state_   = State::IDLE;
    thread_ = std::thread(&WorkerDispatcher::loop, this);
    if (logger_) logger_->info("WorkerDispatcher started");
}

void WorkerDispatcher::stop() {
    running_ = false;
    if (queue_) queue_->close();
    if (logger_) logger_->info("WorkerDispatcher stop requested");
}

void WorkerDispatcher::join() {
    if (thread_.joinable()) thread_.join();
}

void WorkerDispatcher::loop() {
    while (running_) {
        state_ = State::IDLE;
        if (!queue_) break;
        ThreadMessage msg{};
        if (!queue_->pop(msg)) break;   // closed
        state_ = State::RUNNING;
        if (handler_) handler_(msg);
    }
    state_ = State::IDLE;
    if (logger_) logger_->info("WorkerDispatcher loop end");
}

} // namespace device_reminder
