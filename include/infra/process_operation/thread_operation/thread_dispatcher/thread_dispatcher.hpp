#pragma once
#include "infra/process_operation/thread_operation/thread_dispatcher/i_thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include <unordered_map>
#include <functional>
#include <memory>

namespace device_reminder {

class ThreadDispatcher : public IThreadDispatcher {
public:
    using HandlerMap = std::unordered_map<MessageType, std::function<void(const ThreadMessage&)>>;

    ThreadDispatcher(std::shared_ptr<ILogger> logger,
                     HandlerMap handler_map);

    void dispatch(const ThreadMessage& msg) override;

private:
    std::shared_ptr<ILogger> logger_;
    HandlerMap handler_map_;
};

} // namespace device_reminder
