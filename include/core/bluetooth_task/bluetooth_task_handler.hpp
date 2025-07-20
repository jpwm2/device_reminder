#pragma once

#include "interfaces/i_message_handler.hpp"
#include "bluetooth_task/bluetooth_task.hpp"
#include <memory>

namespace device_reminder {

class BluetoothTaskHandler : public IMessageHandler {
public:
    explicit BluetoothTaskHandler(std::shared_ptr<BluetoothTask> task)
        : task_(std::move(task)) {}

    void handle(const std::string& msg_str) override;

private:
    std::shared_ptr<BluetoothTask> task_;
};

} // namespace device_reminder
