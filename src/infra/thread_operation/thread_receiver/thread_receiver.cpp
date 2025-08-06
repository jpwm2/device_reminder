#include "infra/thread_operation/thread_receiver/thread_receiver.hpp"
#include <exception>
#include <utility>

namespace device_reminder {

ThreadReceiver::ThreadReceiver(std::shared_ptr<ILogger> logger,
                               std::shared_ptr<IThreadQueue> queue,
                               std::shared_ptr<IThreadDispatcher> dispatcher)
    : logger_(std::move(logger)),
      queue_(std::move(queue)),
      dispatcher_(std::move(dispatcher)) {}

void ThreadReceiver::run() {
    if (worker_.joinable()) {
        return;
    }
    running_ = true;
    if (logger_) {
        logger_->info("ThreadReceiver started.");
    }
    worker_ = std::thread([this] {
        while (running_) {
            auto msg = queue_ ? queue_->pop() : nullptr;
            if (!msg) {
                if (logger_) {
                    logger_->warn("ThreadReceiver received null message.");
                }
                continue;
            }
            try {
                if (dispatcher_) {
                    dispatcher_->dispatch(msg);
                }
            } catch (const std::exception &e) {
                if (logger_) {
                    logger_->error(std::string("Dispatch error: ") + e.what());
                }
            } catch (...) {
                if (logger_) {
                    logger_->error("Dispatch unknown error.");
                }
            }
        }
        if (logger_) {
            logger_->info("ThreadReceiver stopped.");
        }
    });
}

void ThreadReceiver::stop() {
    running_ = false;
    if (queue_) {
        queue_->push(nullptr);
    }
    if (worker_.joinable()) {
        worker_.join();
    }
}

} // namespace device_reminder
