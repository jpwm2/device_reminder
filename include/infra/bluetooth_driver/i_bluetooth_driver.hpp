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

    /// 周囲のスマートフォン名一覧を取得する
    /// 失敗時は BluetoothDriverError を throw
    virtual std::vector<std::string> scan() = 0;
};

} // namespace device_reminder
