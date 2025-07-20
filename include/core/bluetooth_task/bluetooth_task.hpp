#pragma once

#include "message/i_message.hpp"
#include "bluetooth_task/i_bluetooth_task.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class BluetoothTask : public IBluetoothTask {
public:
    explicit BluetoothTask(std::shared_ptr<ILogger> logger);
    ~BluetoothTask();

    void run() override;
    bool send_message(const IMessage& msg) override;

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
