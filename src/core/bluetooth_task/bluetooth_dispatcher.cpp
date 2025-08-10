#include "bluetooth_task/bluetooth_handler.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"

namespace device_reminder {

BluetoothHandler::BluetoothHandler(std::shared_ptr<ILogger> logger,
                                   std::shared_ptr<IBluetoothTask> task)
    : logger_(std::move(logger)), task_(std::move(task)) {
    if (logger_) logger_->info("BluetoothHandler created");
}

void BluetoothHandler::handle(std::shared_ptr<IProcessMessage> msg) {
    if (!msg || !task_) return;

    if (msg->type() == ProcessMessageType::RequestBluetoothScan) {
        if (logger_) logger_->info("RequestBluetoothScan");
        task_->on_waiting(msg->payload());
    }
}

} // namespace device_reminder
