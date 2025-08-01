#include "infra/process_operation/process_receiver/process_receiver.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"

#include <utility>

namespace device_reminder {

ProcessReceiver::ProcessReceiver(std::shared_ptr<ILogger> logger,
                                 std::shared_ptr<IProcessQueue> queue,
                                 std::shared_ptr<IProcessDispatcher> dispatcher)
    : logger_(std::move(logger)),
      queue_(std::move(queue)),
      dispatcher_(std::move(dispatcher)) {
    if (logger_) logger_->info("ProcessReceiver created");
}

ProcessReceiver::~ProcessReceiver() {
    stop();
}

void ProcessReceiver::run() {
    if (running_) return;
    running_ = true;
    worker_ = std::thread(&ProcessReceiver::loop, this);
}

void ProcessReceiver::stop() {
    bool was_running = running_;
    running_ = false;
    if (worker_.joinable()) worker_.join();
    if (was_running && logger_) logger_->info("ProcessReceiver stopped");
}

void ProcessReceiver::loop() {
    while (running_) {
        if (!queue_) break;
        auto msg = queue_->pop();
        if (!msg) {
            // キューが空の場合はループを終了する
            break;
        }
        if (!running_) break;
        if (dispatcher_) dispatcher_->dispatch(std::move(msg));
    }
    if (logger_) logger_->info("ProcessReceiver loop end");
}

} // namespace device_reminder
