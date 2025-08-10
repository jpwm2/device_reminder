#pragma once
#include "infra/thread_operation/thread_dispatcher/i_thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_operation/thread_message/thread_message_type.hpp"
#include <unordered_map>
#include <functional>
#include <memory>

namespace device_reminder {

class ThreadDispatcher : public IThreadDispatcher {
public:
    using HandlerMap =
        std::unordered_map<ThreadMessageType,
                           std::function<void(std::shared_ptr<IThreadMessage>)>>;

    ThreadDispatcher(std::shared_ptr<ILogger> logger, HandlerMap handler_map);

    void dispatch(std::shared_ptr<IThreadMessage> msg) override;

private:
    std::shared_ptr<ILogger> logger_;
    HandlerMap handler_map_;
};

} // namespace device_reminder
