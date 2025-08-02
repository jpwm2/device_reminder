#pragma once

#include <vector>
#include <string>
#include <stdexcept>

namespace device_reminder {

class BluetoothDriverError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class IBluetoothDriver {
public:
    virtual ~IBluetoothDriver() = default;

    /// Bluetoothスキャンを開始し、検知と通知処理を行う
    virtual void run() = 0;

    /// Bluetoothスキャン処理を停止する
    virtual void stop() = 0;

    /// 既存コードとの互換性のために残したスキャンインタフェース
    /// 失敗時は BluetoothDriverError を throw
    virtual std::vector<std::string> scan() { return {}; }
};

} // namespace device_reminder
