#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

namespace device_reminder {

struct DeviceInfo {
    std::string addr;       ///< MAC アドレス
    int8_t      rssi;       ///< 受信強度 [dBm]
    double      est_distance; ///< 推定距離 [m]
};

class BluetoothDriverError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class IBluetoothDriver {
public:
    virtual ~IBluetoothDriver() = default;

    /// 半径 radius_m 以内に検知したデバイス一覧を返す
    /// 失敗時は BluetoothDriverError を throw
    virtual std::vector<DeviceInfo> scan_once(double radius_m) = 0;
};

} // namespace device_reminder
