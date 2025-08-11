#include "infra/message/message_receiver.hpp"
#include "infra/message/message.hpp"

#include <stdexcept>
#include <utility>
#include <vector>

namespace device_reminder {

MessageReceiver::MessageReceiver(std::shared_ptr<ILogger> logger,
                                 std::shared_ptr<IMessageQueue> queue,
                                 std::shared_ptr<IMessageDispatcher> dispatcher)
    : logger_(std::move(logger)),
      queue_(std::move(queue)),
      dispatcher_(std::move(dispatcher)) {}

void MessageReceiver::run() {
    if (logger_) logger_->info("MessageReceiver run start");
    if (!queue_ || !dispatcher_) {
        if (logger_) logger_->error("MessageReceiver run failed: dependency null");
        throw std::runtime_error("dependency null");
    }

    running_ = true;
    thread_ = std::thread([this] {
        try {
            while (running_) {
                auto msg = queue_->pop();
                if (!running_) break;
                if (logger_) logger_->info("MessageReceiver received: " + msg->to_string());
                dispatcher_->dispatch(msg);
                if (logger_) logger_->info("MessageReceiver dispatch success");
            }
        } catch (const std::exception &e) {
            if (logger_) logger_->error(std::string("MessageReceiver thread error: ") + e.what());
        }
        if (logger_) logger_->info("MessageReceiver thread end");
    });
    if (logger_) logger_->info("MessageReceiver run success");
}

void MessageReceiver::stop() {
    if (logger_) logger_->info("MessageReceiver stop start");
    running_ = false;
    try {
        if (queue_) {
            queue_->push(
                std::make_shared<Message>(MessageType::None, std::vector<std::string>{}));
        }
        if (thread_.joinable()) {
            thread_.join();
        }
        if (logger_) logger_->info("MessageReceiver stop success");
    } catch (const std::exception& e) {
        if (logger_)
            logger_->error(std::string("MessageReceiver stop error: ") + e.what());
        throw;
    }
}

} // namespace device_reminder
