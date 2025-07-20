#include "worker_dispatcher/worker_dispatcher.hpp"
#include "message/message.hpp"
#include "infra/logger/i_logger.hpp"

#include <chrono>
#include <thread>

namespace device_reminder {

WorkerDispatcher::WorkerDispatcher(key_t, MessageHandler handler, long, std::shared_ptr<ILogger> logger)
    : handler_(std::move(handler)), logger_(std::move(logger)) {
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
    thread_ = std::thread(&WorkerDispatcher::loop, this);
    if (logger_) logger_->info("WorkerDispatcher started");
}

void WorkerDispatcher::stop() {
    running_ = false;
    if (logger_) logger_->info("WorkerDispatcher stop requested");
}

void WorkerDispatcher::join() {
    if (thread_.joinable()) thread_.join();
}

void WorkerDispatcher::loop() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (handler_) handler_(Message{});
    }
    if (logger_) logger_->info("WorkerDispatcher loop end");
}

} // namespace device_reminder
