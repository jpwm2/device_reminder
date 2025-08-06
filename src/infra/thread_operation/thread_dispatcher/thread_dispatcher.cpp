#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include <stdexcept>
#include <utility>

namespace device_reminder {

ThreadDispatcher::ThreadDispatcher(std::shared_ptr<ILogger> logger,
                                   HandlerMap handler_map)
    : logger_(std::move(logger)), handler_map_(std::move(handler_map)) {}

void ThreadDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) logger_->error("msg is nullptr");
        throw std::invalid_argument("msg is nullptr");
    }
    auto type = msg->type();
    auto it = handler_map_.find(type);
    if (it == handler_map_.end()) {
        if (logger_) logger_->warn("Unknown MessageType: " +
                                   std::to_string(static_cast<int>(type)));
        return;
    }

    it->second(std::move(msg));
    if (logger_)
        logger_->info("Dispatch success for type: " +
                       std::to_string(static_cast<int>(type)));
}

} // namespace device_reminder
