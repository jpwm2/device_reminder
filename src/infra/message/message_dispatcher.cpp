#include "infra/message/message_dispatcher.hpp"

#include <exception>
#include <string>
#include <utility>

namespace device_reminder {

IMessageDispatcher::IMessageDispatcher(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

MessageDispatcher::MessageDispatcher(std::shared_ptr<ILogger> logger)
    : IMessageDispatcher(std::move(logger)) {}

void MessageDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    try {
        if (logger_) {
            logger_->info("dispatch start: " +
                           (msg ? msg->to_string() : std::string("null message")));
        }

        if (logger_) {
            logger_->info("dispatch success");
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error(std::string("dispatch failed: ") + e.what());
        }
        throw;
    } catch (...) {
        if (logger_) {
            logger_->error("dispatch failed: unknown exception");
        }
        throw;
    }
}

} // namespace device_reminder

