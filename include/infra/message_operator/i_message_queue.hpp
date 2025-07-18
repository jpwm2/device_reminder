#pragma once
#include <cstdint>
#include <optional>

namespace device_reminder {

class IMessageQueue {
public:
    virtual ~IMessageQueue() = default;
    IMessageQueue(const IMessageQueue&)            = delete;
    IMessageQueue& operator=(const IMessageQueue&) = delete;

    /// ブロッキング送信（成功で true）
    virtual bool push(uint32_t msg) = 0;

    /// ブロッキング受信（キューが閉じられていたら std::nullopt）
    virtual std::optional<uint32_t> pop() = 0;

    /// 受信値を out に格納（成功で true）
    virtual bool pop(uint32_t& out) = 0;

    /// キューが有効なら true
    virtual bool is_open() const noexcept = 0;

    /// 明示的に close（他プロセスが使わない場合は unlink も）
    virtual void close() = 0;
};

} // namespace device_reminder
