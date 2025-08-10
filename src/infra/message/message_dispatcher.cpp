#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include <utility>

namespace device_reminder {

ThreadDispatcher::ThreadDispatcher(std::shared_ptr<ILogger> logger,
                                   HandlerMap handler_map)
    : logger_(std::move(logger)),
      handler_map_(std::move(handler_map)) {
    if (logger_) logger_->info("ThreadDispatcher created");
}

void ThreadDispatcher::dispatch(std::shared_ptr<IThreadMessage> msg) {
    if (!msg) {
        if (logger_) logger_->error("Null thread message");
        return;
    }
    auto it = handler_map_.find(msg->type());
    if (it != handler_map_.end()) {
        it->second(std::move(msg));
    } else {
        if (logger_) logger_->info("Unhandled thread message");
    }
}

} // namespace device_reminder
