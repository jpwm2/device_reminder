#pragma once

#include "infra/logger/i_logger.hpp"
#include "human_task/human_handler.hpp"
#include "infra/message/message.hpp"

#include <memory>

namespace device_reminder {

class IHumanDispatcher {
public:
    IHumanDispatcher(std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IHumanHandler> handler,
                     MessageType message_type);
    virtual ~IHumanDispatcher() = default;

    virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;

protected:
    std::shared_ptr<ILogger>       logger_;
    std::shared_ptr<IHumanHandler> handler_;
    MessageType                    message_type_;
};

class HumanDispatcher : public IHumanDispatcher {
public:
    HumanDispatcher(std::shared_ptr<ILogger> logger,
                    std::shared_ptr<IHumanHandler> handler,
                    MessageType message_type);

    void dispatch(std::shared_ptr<IMessage> msg) override;

private:
    enum class State { Detecting, Stopped };
    State state_{State::Detecting};

    const char* state_to_string(State state) const;
};

} // namespace device_reminder

