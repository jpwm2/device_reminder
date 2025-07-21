#include "bluetooth_task/bluetooth_task.hpp"
#include "infra/i_message.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "process_message_operation/i_process_message_sender.hpp"

namespace device_reminder {

BluetoothTask::BluetoothTask(std::shared_ptr<IBluetoothDriver> driver,
                             std::shared_ptr<IProcessMessageSender> sender,
                             std::shared_ptr<ILogger> logger)
    : driver_(std::move(driver))
    , sender_(std::move(sender))
    , logger_(std::move(logger)) {
    if (logger_) logger_->info("BluetoothTask created");
}

BluetoothTask::~BluetoothTask() {
    if (logger_) logger_->info("BluetoothTask destroyed");
}

void BluetoothTask::run(const IMessage& msg) {
    if (msg.type() != MessageType::BluetoothScanRequest) return;

    state_ = State::Scanning;
    bool detected = false;
    try {
        if (driver_) {
            auto devices = driver_->scan();
            detected = !devices.empty();
        }
    } catch (const std::exception& e) {
        if (logger_) logger_->error(e.what());
    }

    if (sender_) {
        sender_->enqueue(ProcessMessage{MessageType::DevicePresenceResponse, detected});
    }

    state_ = State::WaitRequest;
}

bool BluetoothTask::send_message(const IMessage& msg) {
    run(msg);
    return true;
}

} // namespace device_reminder
