#pragma once

#include "infra/logger.hpp"
#include "core/main_task/main_handler.hpp"
#include "infra/message/message.hpp"

#include <memory>
#include <string>

namespace device_reminder {

// 状態定義
enum class State {
    WaitingHumanDetection,
    WaitingDevicePresenceResponse,
    WaitingDevicePresenceRetryResponse,
    BluetoothScanRequestCooldown,
};

using IMessageType = State;

class IMainDispatcher {
public:
    virtual ~IMainDispatcher() = default;
    virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;
};

class MainDispatcher : public IMainDispatcher {
public:
    MainDispatcher(std::shared_ptr<ILogger> logger,
                   std::shared_ptr<IMainHandler> handler,
                   IMessageType initial_state);

    void dispatch(std::shared_ptr<IMessage> msg) override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IMainHandler> handler_;
    IMessageType                state_;
};

} // namespace device_reminder

