#include "infra/message/message_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"

#include <stdexcept>
#include <utility>

namespace device_reminder {

MessageDispatcher::MessageDispatcher(std::shared_ptr<ILogger> logger,
                                     HandlerMap handler_map)
    : logger_(std::move(logger)),
      handler_map_(std::move(handler_map)) {}

void MessageDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) logger_->error("dispatch: msg is null");
        throw std::invalid_argument("msg is null");
    }

    auto type = msg->type();
    auto payload = msg->payload();

    if (logger_) {
        if (!payload.empty()) {
            logger_->info("dispatch: type=" +
                         std::to_string(static_cast<int>(type)) +
                         ", payload_size=" +
                         std::to_string(payload.size()));
        } else {
            logger_->info("dispatch: type=" +
                         std::to_string(static_cast<int>(type)));
        }
    }

    auto it = handler_map_.find(type);
    if (it == handler_map_.end()) {
        if (logger_) {
            logger_->error("dispatch: no handler for type=" +
                           std::to_string(static_cast<int>(type)));
        }
        throw std::runtime_error("no handler");
    }

    auto &handler = it->second;

    try {
        handler(payload);
    } catch (const std::exception &e) {
        if (logger_) {
            logger_->error("dispatch failed: type=" +
                           std::to_string(static_cast<int>(type)) +
                           ", what=" + e.what());
        }
        throw;
    } catch (...) {
        if (logger_) {
            logger_->error("dispatch failed: type=" +
                           std::to_string(static_cast<int>(type)) +
                           ", unknown exception");
        }
        throw;
    }

    if (logger_) {
        if (!payload.empty()) {
            logger_->info("dispatch: succeeded, type=" +
                         std::to_string(static_cast<int>(type)) +
                         ", payload_size=" +
                         std::to_string(payload.size()));
        } else {
            logger_->info("dispatch: succeeded, type=" +
                         std::to_string(static_cast<int>(type)));
        }
    }
}

} // namespace device_reminder

