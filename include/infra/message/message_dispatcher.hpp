#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"

#include <memory>

namespace device_reminder {

class IMessageDispatcher {
public:
    explicit IMessageDispatcher(std::shared_ptr<ILogger> logger);
    virtual ~IMessageDispatcher() = default;
    virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;

protected:
    std::shared_ptr<ILogger> logger_{};
};

class MessageDispatcher : public IMessageDispatcher {
public:
    explicit MessageDispatcher(std::shared_ptr<ILogger> logger);

    void dispatch(std::shared_ptr<IMessage> msg) override;
};

} // namespace device_reminder

