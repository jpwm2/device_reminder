#include "bluetooth_task/bluetooth_task_handler.hpp"

namespace device_reminder {

void BluetoothTaskHandler::handle(const Message& msg) {
    if (task_) task_->run(msg);
}

} // namespace device_reminder
