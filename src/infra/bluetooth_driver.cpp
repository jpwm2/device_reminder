#include "bluetooth_driver/bluetooth_driver.hpp"

#include <cmath>

namespace device_reminder {

BluetoothDriver::BluetoothDriver(std::shared_ptr<IBluetoothScanner> scanner,
                                 std::shared_ptr<ILogger> logger)
    : scanner_(std::move(scanner)), logger_(std::move(logger)) {}

std::vector<DeviceInfo> BluetoothDriver::scan_once(double radius_m) {
    std::lock_guard lk{mtx_};
    if (!scanner_) throw BluetoothDriverError("scanner not initialized");

    std::vector<AdvertisementInfo> advs;
    try {
        advs = scanner_->scan();
    } catch (const std::exception& ex) {
        throw BluetoothDriverError(ex.what());
    }

    std::vector<DeviceInfo> results;
    for (const auto& adv : advs) {
        int8_t txp = adv.tx_power.value_or(-59);
        double dist = rssi_to_distance(adv.rssi, txp);
        if (dist <= radius_m) {
            results.push_back(DeviceInfo{adv.addr, adv.rssi, dist});
        }
    }
    return results;
}

double BluetoothDriver::rssi_to_distance(int8_t rssi, int8_t tx_power, double n) {
    return std::pow(10.0, (tx_power - rssi) / (10.0 * n));
}

} // namespace device_reminder
