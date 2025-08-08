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
      dispatcher_(std::move(dispatcher)) {}

ProcessReceiver::~ProcessReceiver() {
    stop();
}

void ProcessReceiver::run() {
    if (running_) return;
    running_ = true;
    if (logger_) logger_->info("受信スレッド開始");
    worker_ = std::thread(&ProcessReceiver::loop, this);
}

void ProcessReceiver::stop() {
    if (!running_) return;
    running_ = false;
    if (worker_.joinable()) worker_.join();
    if (logger_) logger_->info("受信スレッド停止完了");
}

void ProcessReceiver::loop() {
    while (running_) {
        if (!queue_) break;
        auto msg = queue_->pop();
        if (!msg) {
            if (logger_) logger_->warn("受信メッセージがnullptrのため無視");
            continue;
        }
        if (dispatcher_) {
            dispatcher_->dispatch(std::move(msg));
            if (logger_) logger_->info("メッセージをディスパッチしました");
        }
    }
    if (logger_) logger_->info("受信スレッド停止");
}

} // namespace device_reminder

