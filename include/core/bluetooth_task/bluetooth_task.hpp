#pragma once

#include "message/i_message.hpp"
#include "bluetooth_task/i_bluetooth_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/message_operator/i_message_sender.hpp"
#include <memory>

namespace device_reminder {

class BluetoothTask : public IBluetoothTask {
public:
    enum class State { WaitRequest, Scanning };

    BluetoothTask(std::shared_ptr<IBluetoothDriver> driver,
                  std::shared_ptr<IMessageSender> sender,
                  std::shared_ptr<ILogger> logger = nullptr);
    ~BluetoothTask();

    void run(const IMessage& msg) override;
    bool send_message(const IMessage& msg) override;

    State state() const noexcept { return state_; }

private:
    std::shared_ptr<IBluetoothDriver> driver_;
    std::shared_ptr<IMessageSender> sender_;
    std::shared_ptr<ILogger> logger_;
    State state_{State::WaitRequest};
};

} // namespace device_reminder
