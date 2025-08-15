#include "bluetooth_task/bluetooth_handler.hpp"

namespace device_reminder {

BluetoothHandler::BluetoothHandler(std::shared_ptr<ILogger> logger,
                                   std::shared_ptr<IFileLoader> loader,
                                   std::shared_ptr<IProcessSender> sender,
                                   std::shared_ptr<IBluetoothDriver> driver,
                                   std::string main_endpoint,
                                   std::shared_ptr<IMessage> success_msg,
                                   std::shared_ptr<IMessage> failure_msg)
    : logger_(std::move(logger))
    , loader_(std::move(loader))
    , sender_(std::move(sender))
    , driver_(std::move(driver))
    , main_endpoint_(std::move(main_endpoint))
    , success_msg_(std::move(success_msg))
    , failure_msg_(std::move(failure_msg)) {}

std::vector<std::string> BluetoothHandler::scan() {
    if (logger_) logger_->info("[BluetoothHandler::scan] start");
    try {
        if (loader_) {
            loader_->load_string_list("device_list");
        }

        std::vector<std::string> devices;
        if (driver_) {
            devices = driver_->scan();
        }

        if (sender_ && !main_endpoint_.empty()) {
            if (!devices.empty() && success_msg_) {
                sender_->send(main_endpoint_, success_msg_);
            } else if (devices.empty() && failure_msg_) {
                sender_->send(main_endpoint_, failure_msg_);
            }
        }

        if (logger_) {
            logger_->info("[BluetoothHandler::scan] success: " + std::to_string(devices.size()));
        }

        return devices;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error(std::string("[BluetoothHandler::scan] failed: ") + e.what());
        }
        throw;
    }
}

} // namespace device_reminder

