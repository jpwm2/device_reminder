#pragma once
#include <cstdint>
#include "message/message.hpp"

namespace device_reminder {

class ITimerService {
public:
    virtual ~ITimerService() = default;

    /// ワンショットタイマーを開始
    /// @param milliseconds  タイムアウトまでの時間 [ms]
    /// @param timeout_msg   タイムアウト時に送信するメッセージ
    virtual void start(uint32_t milliseconds,
                       Message timeout_msg) = 0;

    /// タイマー停止（起動していない場合は無視）
    virtual void stop() = 0;

    /// 動作中かどうか
    virtual bool active() const noexcept = 0;
};

} // namespace device_reminder
