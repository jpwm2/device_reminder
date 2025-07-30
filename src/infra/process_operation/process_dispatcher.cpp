#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"

namespace device_reminder {

ProcessDispatcher::ProcessDispatcher(std::shared_ptr<ILogger> logger,
                                     HandlerMap handler_map)
    : logger_(std::move(logger)), handler_map_(std::move(handler_map)) {
    if (logger_) logger_->info("ProcessDispatcher created");
}

void ProcessDispatcher::dispatch(std::shared_ptr<IProcessMessage> msg) {
    if (!msg) {
        if (logger_) logger_->error("Null process message");
        return;
    }
    auto it = handler_map_.find(msg->type());
    if (it != handler_map_.end()) {
        it->second(std::move(msg));
    } else if (logger_) {
        logger_->warn("Unhandled process message");
    }
}

} // namespace device_reminder

