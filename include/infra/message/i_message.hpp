#pragma once

namespace device_reminder {

// メッセージタイプ
enum class MessageType {
    None,
    Timeout,
    HumanDetected,
    BluetoothEvent,
    BuzzerOn,
    BuzzerOff
};

// メッセージの抽象インターフェース
class IMessage {
public:
    virtual ~IMessage() = default;

    // メッセージタイプを取得
    virtual MessageType type() const = 0;

    // ペイロードを取得
    virtual bool payload() const = 0;
};

} // namespace device_reminder
