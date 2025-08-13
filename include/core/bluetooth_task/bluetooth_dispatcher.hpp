#pragma once

#include "infra/logger.hpp"
#include "infra/message/message.hpp"

#include <memory>

namespace device_reminder {

class IBluetoothHandler;

class IBluetoothDispatcher {
public:
    IBluetoothDispatcher(std::shared_ptr<ILogger> logger,
                         std::shared_ptr<IBluetoothHandler> handler,
                         MessageType message_type);
    virtual ~IBluetoothDispatcher() = default;

    virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;

protected:
    std::shared_ptr<ILogger>          logger_;
    std::shared_ptr<IBluetoothHandler> handler_;
    MessageType                       message_type_;
};

class BluetoothDispatcher : public IBluetoothDispatcher {
public:
    BluetoothDispatcher(std::shared_ptr<ILogger> logger,
                        std::shared_ptr<IBluetoothHandler> handler,
                        MessageType message_type);

    void dispatch(std::shared_ptr<IMessage> msg) override;

private:
    enum class State { Ready, Cooldown };
    State state_{State::Ready};

    const char* state_to_string(State state) const;
};

} // namespace device_reminder

