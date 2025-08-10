#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace device_reminder {

class IMessageDispatcher {
public:
    virtual ~IMessageDispatcher() = default;
    virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;
};

class MessageDispatcher : public IMessageDispatcher {
public:
    using Handler = std::function<void(std::vector<std::string>)>;
    using HandlerMap = std::unordered_map<MessageType, Handler>;

    MessageDispatcher(std::shared_ptr<ILogger> logger,
                      HandlerMap handler_map);

    void dispatch(std::shared_ptr<IMessage> msg) override;

private:
    std::shared_ptr<ILogger> logger_;
    HandlerMap handler_map_;
};

} // namespace device_reminder

