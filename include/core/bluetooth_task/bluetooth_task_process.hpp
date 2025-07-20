#pragma once

#include "infra/process/process_base.hpp"
#include "bluetooth_task/bluetooth_task_handler.hpp"
#include <memory>

namespace device_reminder {

class BluetoothTaskProcess : public ProcessBase {
public:
    BluetoothTaskProcess(const std::string& mq_name,
                         std::shared_ptr<BluetoothTask> task,
                         std::shared_ptr<ILogger> logger)
        : ProcessBase(mq_name,
                      std::make_shared<BluetoothTaskHandler>(std::move(task)),
                      std::move(logger),
                      2) {}
};

} // namespace device_reminder
