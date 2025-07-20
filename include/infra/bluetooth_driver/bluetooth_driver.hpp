#pragma once

#include "i_bluetooth_driver.hpp"
#include "infra/logger/i_logger.hpp"

#include <optional>
#include <memory>
#include <mutex>

namespace device_reminder {

struct AdvertisementInfo {
    std::string addr;
    int8_t      rssi;
    std::optional<int8_t> tx_power;  ///< 広告から取得したTxPower, なければnullopt
};

class IBluetoothScanner {
public:
    virtual ~IBluetoothScanner() = default;
    virtual std::vector<AdvertisementInfo> scan() = 0;  ///< 1回だけスキャン
};

class BluetoothDriver : public IBluetoothDriver {
public:
    BluetoothDriver(std::shared_ptr<IBluetoothScanner> scanner,
                    std::shared_ptr<ILogger> logger = nullptr);

    std::vector<DeviceInfo> scan_once(double radius_m) override;

private:
    static double rssi_to_distance(int8_t rssi, int8_t tx_power, double n = 2.0);

    std::shared_ptr<IBluetoothScanner> scanner_;
    std::shared_ptr<ILogger> logger_;
    std::mutex mtx_;
};

} // namespace device_reminder
