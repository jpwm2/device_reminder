#include "infra/message/message_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"

#include <spdlog/fmt/fmt.h>
#include <stdexcept>
#include <utility>

namespace device_reminder {

MessageDispatcher::MessageDispatcher(
    std::shared_ptr<ILogger> logger,
    std::unordered_map<MessageType, std::function<void(std::vector<std::string>)>>
        handler_map)
    : logger_(std::move(logger)),
      handler_map_(std::move(handler_map)) {}

void MessageDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) logger_->error("dispatch: msg is null");
        throw std::invalid_argument("msg is null");
    }

    auto type = msg->type();
    auto payload = msg->payload();
    auto payload_size = payload.size();

    if (logger_) {
        if (payload_size) {
            logger_->info(fmt::format("dispatch: type={}, payload_size={}",
                                      static_cast<int>(type), payload_size));
        } else {
            logger_->info(
                fmt::format("dispatch: type={}", static_cast<int>(type)));
        }
    }

    auto it = handler_map_.find(type);
    if (it == handler_map_.end()) {
        if (logger_) {
            logger_->error(fmt::format("dispatch: no handler for type={}",
                                       static_cast<int>(type)));
        }
        throw std::runtime_error("no handler");
    }

    auto &handler = it->second;

    try {
        handler(payload);
    } catch (const std::exception &e) {
        if (logger_) {
            logger_->error(fmt::format("dispatch failed: type={}, what={}",
                                       static_cast<int>(type), e.what()));
        }
        throw;
    } catch (...) {
        if (logger_) {
            logger_->error(fmt::format(
                "dispatch failed: type={}, unknown exception",
                static_cast<int>(type)));
        }
        throw;
    }

    if (logger_) {
        if (payload_size) {
            logger_->info(fmt::format(
                "dispatch: succeeded, type={}, payload_size={}",
                static_cast<int>(type), payload_size));
        } else {
            logger_->info(fmt::format("dispatch: succeeded, type={}",
                                      static_cast<int>(type)));
        }
    }
}

} // namespace device_reminder

