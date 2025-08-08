#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/message/i_message.hpp"

#include <stdexcept>
#include <string>

namespace device_reminder {

ProcessDispatcher::ProcessDispatcher(std::shared_ptr<ILogger> logger,
                                     HandlerMap handler_map)
    : logger_(std::move(logger)), handler_map_(std::move(handler_map)) {}

void ProcessDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) logger_->error("Null message");
        throw std::invalid_argument("msg is null");
    }

    auto type = msg->type();
    auto it   = handler_map_.find(type);
    if (it != handler_map_.end() && it->second) {
        it->second(msg);
        if (logger_) {
            logger_->info("Dispatched message type " +
                          std::to_string(static_cast<int>(type)));
        }
    } else if (logger_) {
        logger_->warn("Unhandled message type " +
                      std::to_string(static_cast<int>(type)));
    }
}

} // namespace device_reminder

