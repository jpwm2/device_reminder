#include "bluetooth_task/bluetooth_handler.hpp"

namespace device_reminder {

BluetoothHandler::BluetoothHandler(std::shared_ptr<ILogger> logger,
                                   std::shared_ptr<IFileLoader> loader,
                                   std::shared_ptr<IProcessSender> sender,
                                   std::shared_ptr<IBluetoothDriver> driver,
                                   std::shared_ptr<IMessageQueue> main_queue,
                                   std::shared_ptr<IMessage> success_msg,
                                   std::shared_ptr<IMessage> failure_msg)
    : logger_(std::move(logger))
    , loader_(std::move(loader))
    , sender_(std::move(sender))
    , driver_(std::move(driver))
    , main_queue_(std::move(main_queue))
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

        if (sender_ && main_queue_) {
            if (!devices.empty() && success_msg_) {
                sender_->send(main_queue_, success_msg_);
            } else if (devices.empty() && failure_msg_) {
                sender_->send(main_queue_, failure_msg_);
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

