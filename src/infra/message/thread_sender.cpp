#include "infra/message/thread_sender.hpp"

#include <stdexcept>
#include <utility>

namespace device_reminder {

ThreadSender::ThreadSender(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {
    if (logger_) {
        logger_->info("ThreadSender created");
    }
}

void ThreadSender::send(std::shared_ptr<IMessageQueue> queue,
                        std::shared_ptr<IMessage> message) {
    if (logger_) {
        logger_->info("ThreadSender send start");
    }
    try {
        if (!queue) {
            if (logger_) {
                logger_->error("ThreadSender send failed: queue is null");
            }
            throw std::invalid_argument("queue is null");
        }
        if (!message) {
            if (logger_) {
                logger_->error("ThreadSender send failed: message is null");
            }
            throw std::invalid_argument("message is null");
        }

        if (logger_) {
            logger_->info("ThreadSender send message: " + message->to_string());
        }

        queue->push(std::move(message));

        if (logger_) {
            logger_->info("ThreadSender send success");
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error(std::string("ThreadSender send exception: ") + e.what());
        }
        throw;
    }
}

} // namespace device_reminder

