#pragma once
#include "infra/thread_operation/thread_dispatcher/i_thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/message/i_message.hpp"
#include <functional>
#include <memory>
#include <unordered_map>

namespace device_reminder {

class ThreadDispatcher : public IThreadDispatcher {
public:
    using HandlerMap =
        std::unordered_map<MessageType,
                           std::function<void(std::shared_ptr<IMessage>)>>;

    ThreadDispatcher(std::shared_ptr<ILogger> logger, HandlerMap handler_map);

    void dispatch(std::shared_ptr<IMessage> msg) override;

private:
    std::shared_ptr<ILogger> logger_;
    HandlerMap handler_map_;
};

} // namespace device_reminder
