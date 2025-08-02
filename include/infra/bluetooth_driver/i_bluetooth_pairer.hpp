#pragma once

#include <string>

namespace device_reminder {

class IBluetoothPairer {
public:
    virtual ~IBluetoothPairer() = default;
    /// 指定したデバイスとのペアリングを試みる
    /// 成功した場合は true を返す
    virtual bool pair(const std::string& mac) = 0;
};

} // namespace device_reminder
