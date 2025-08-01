#include "bluetooth_task/bluetooth_task.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/bluetooth_driver/i_bluetooth_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include <algorithm>

namespace device_reminder {

BluetoothTask::BluetoothTask(std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IProcessSender> sender,
                             std::shared_ptr<IFileLoader> loader,
                             std::shared_ptr<IBluetoothDriver> driver)
    : logger_(std::move(logger))
    , sender_(std::move(sender))
    , loader_(std::move(loader))
    , driver_(std::move(driver)) {
    if (logger_) logger_->info("BluetoothTask created");
}

BluetoothTask::~BluetoothTask() {
    if (logger_) logger_->info("BluetoothTask destroyed");
}

void BluetoothTask::on_waiting(const std::vector<std::string>&) {

    state_ = State::Scanning;
    bool detected = false;
    try {
        if (driver_) {
            auto devices = driver_->scan();
            if (loader_) {
                auto regs = loader_->load_string_list("device_list");
                for (const auto& name : devices) {
                    if (std::find(regs.begin(), regs.end(), name) != regs.end()) {
                        detected = true;
                        break;
                    }
                }
            } else {
                detected = !devices.empty();
            }
        }
    } catch (const std::exception& e) {
        if (logger_) logger_->error(e.what());
    }

    if (detected && sender_) {
        sender_->send();
    }

    state_ = State::WaitRequest;
}

} // namespace device_reminder
