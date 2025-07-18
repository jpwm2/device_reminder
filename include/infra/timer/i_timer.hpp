#pragma once

#include <cstdint>

namespace device_reminder {

class ITimer {
public:
    virtual ~ITimer() = default;

    // タイマーを開始する
    virtual void start() = 0;

    // タイマーを停止する
    virtual void stop() = 0;
};

} // namespace device_reminder
