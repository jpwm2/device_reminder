#include "bluetooth_task.hpp"
#include "message/i_message.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

BluetoothTask::BluetoothTask(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {
    if (logger_) logger_->info("BluetoothTask created");
}

BluetoothTask::~BluetoothTask() {
    if (logger_) logger_->info("BluetoothTask destroyed");
}

void BluetoothTask::run() {
    if (logger_) logger_->info("BluetoothTask running");
}

bool BluetoothTask::send_message(const IMessage& msg) {
    if (logger_) logger_->info("BluetoothTask send_message");
    return true;
}

} // namespace device_reminder
