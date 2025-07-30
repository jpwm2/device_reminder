#pragma once
#include <memory>

namespace device_reminder {

class IProcessReceiver {
public:
    virtual ~IProcessReceiver() = default;

    /**
     * @brief 受信スレッドを起動しメインループを開始する
     */
    virtual void run() = 0;

    /**
     * @brief スレッドの停止を指示する
     */
    virtual void stop() = 0;
};

} // namespace device_reminder
