#pragma once

#include <string>

namespace device_reminder {

class IGPIODriver {
public:
    virtual ~IGPIODriver() = default;

    // GPIOチップをオープン
    virtual void openChip(const std::string& chipName) = 0;

    // GPIOラインを初期化
    virtual void setupLine(unsigned int lineNumber) = 0;

    // ラインの状態を取得（0または1）
    virtual int readLine() = 0;

    // リソースのクリーンアップ
    virtual void close() = 0;
};

} // namespace device_reminder
