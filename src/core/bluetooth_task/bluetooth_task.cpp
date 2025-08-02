#include "bluetooth_task/bluetooth_task.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"

namespace device_reminder {

BluetoothTask::BluetoothTask(std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IProcessSender> sender,
                             std::shared_ptr<IBluetoothDriver> driver)
    : logger_(std::move(logger)), sender_(std::move(sender)),
      driver_(std::move(driver)) {
  if (logger_)
    logger_->info("BluetoothTask created");
}

BluetoothTask::~BluetoothTask() {
  if (logger_)
    logger_->info("BluetoothTask destroyed");
}

void BluetoothTask::on_waiting(const std::vector<std::string> &) {

  state_ = State::Scanning;
  try {
    if (driver_) {
      driver_->run();
      if (!driver_->paired_devices().empty() && sender_) {
        sender_->send();
      }
    }
  } catch (const std::exception &e) {
    if (logger_)
      logger_->error(e.what());
  }

  state_ = State::WaitRequest;
}

} // namespace device_reminder
