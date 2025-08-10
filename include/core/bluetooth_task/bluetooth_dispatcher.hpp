#pragma once

#include "core/interfaces/i_handler.hpp"
#include "core/bluetooth_task/i_bluetooth_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_message/i_process_message.hpp"

#include <memory>

namespace device_reminder {

class BluetoothHandler : public IHandler {
public:
    BluetoothHandler(std::shared_ptr<ILogger> logger,
                     std::shared_ptr<IBluetoothTask> task);

    void handle(std::shared_ptr<IProcessMessage> msg) override;

private:
    std::shared_ptr<ILogger>       logger_;
    std::shared_ptr<IBluetoothTask> task_;
};

} // namespace device_reminder
