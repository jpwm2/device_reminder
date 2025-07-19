#pragma once
#include <cstdint>

namespace device_reminder {

class IMessageSender {
public:
    virtual ~IMessageSender() = default;

    /// 送信待ちキューにメッセージを追加（成功: true, 停止中: false）
    virtual bool enqueue(uint32_t msg) = 0;

    /// 送信スレッドを停止し，リソースを解放
    virtual void stop() = 0;
};

} // namespace device_reminder
