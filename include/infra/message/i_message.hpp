#pragma once

namespace device_reminder {

//--------------------------------------
// 列挙: メッセージタイプ
//--------------------------------------
enum class MessageType {
    None,
    Timeout,
    HumanDetected,
    HumanDetectStart,
    HumanDetectStop,
    StartScan,
    DeviceScanResult,
    BluetoothEvent,
    BuzzerOn,
    BuzzerOff,
    BluetoothScanRequest,
    DevicePresenceResponse
};

//--------------------------------------
// 抽象クラス: IMessage
//--------------------------------------
class IMessage {
public:
    virtual ~IMessage() = default;

    /// メッセージタイプを取得
    virtual MessageType type() const noexcept = 0;

    /// ペイロード（true/false）を取得
    virtual bool payload() const noexcept = 0;
};

} // namespace device_reminder
