#include "bluetooth_driver/bluetooth_driver.hpp"

#include <algorithm>

namespace device_reminder {

BluetoothDriver::BluetoothDriver(std::shared_ptr<IFileLoader> device_loader,
                                 std::shared_ptr<IFileLoader> distance_threshold_loader,
                                 std::shared_ptr<IBluetoothScanner> scanner,
                                 std::shared_ptr<IBluetoothPairer> pairer,
                                 std::shared_ptr<IThreadSender> sender,
                                 std::shared_ptr<ILogger> logger)
    : device_loader_(std::move(device_loader))
    , distance_threshold_loader_(std::move(distance_threshold_loader))
    , scanner_(std::move(scanner))
    , pairer_(std::move(pairer))
    , sender_(std::move(sender))
    , logger_(std::move(logger)) {}

void BluetoothDriver::run() {
    running_ = true;
    paired_devices_.clear();

    if (!device_loader_ || !distance_threshold_loader_ || !scanner_ || !pairer_) {
        if (logger_) logger_->warn("BluetoothDriver dependencies not set");
        running_ = false;
        return;
    }

    auto registered = device_loader_->load_string_list("device_list");
    int threshold = distance_threshold_loader_->load_int("distance_threshold");

    auto scanned = scanner_->scan();
    std::vector<std::string> targets;
    for (const auto& dev : scanned) {
        if (std::find(registered.begin(), registered.end(), dev.mac) != registered.end() &&
            dev.rssi >= threshold) {
            targets.push_back(dev.mac);
        }
    }

    for (const auto& mac : targets) {
        try {
            if (pairer_->pair(mac)) {
                paired_devices_.push_back(mac);
            }
        } catch (const std::exception& e) {
            if (logger_) logger_->error(e.what());
        }
    }

    if (!paired_devices_.empty() && sender_) {
        sender_->send();
    }

    running_ = false;
}

void BluetoothDriver::stop() {
    running_ = false;
}

std::vector<std::string> BluetoothDriver::scan() {
    if (!scanner_) return {};
    auto scanned = scanner_->scan();
    std::vector<std::string> names;
    for (const auto& dev : scanned) {
        names.push_back(dev.mac);
    }
    return names;
}

} // namespace device_reminder
