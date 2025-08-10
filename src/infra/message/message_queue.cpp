#include "infra/message/message_queue.hpp"

#include <stdexcept>
#include <utility>

namespace device_reminder {

MessageQueue::MessageQueue(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {
    if (logger_) logger_->info("MessageQueue created");
}

void MessageQueue::push(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) logger_->error("MessageQueue push: null message");
        throw std::invalid_argument("msg is null");
    }

    {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(std::move(msg));
    }
    cv_.notify_one();
    if (logger_) logger_->info("MessageQueue push success");
}

std::shared_ptr<IMessage> MessageQueue::pop() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return !queue_.empty(); });
    auto msg = queue_.front();
    queue_.pop();
    if (logger_) logger_->info("MessageQueue pop success");
    return msg;
}

} // namespace device_reminder

