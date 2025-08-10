#pragma once

namespace infra {

// プロセス／スレッド間で送受信されるメッセージの種別
enum class MessageType {
    None,
    HumanFound,
    StartHumanTask,
    StopHumanTask,
    StartBluetoothTask,
    DeviceFound,
    DeviceNotFound,
    StartBuzzer,
    StopBuzzer,
    Timeout,

    // 既存実装との互換用エイリアス
    Unknown = None,
    HumanDetected = HumanFound,
    StartHumanDetection = StartHumanTask,
    StopHumanDetection = StopHumanTask,
    RequestBluetoothScan = StartBluetoothTask,
    RespondDeviceFound = DeviceFound,
    RespondDeviceNotFound = DeviceNotFound,
    StartBuzzing = StartBuzzer,
    StopBuzzing = StopBuzzer,
    BuzzTimeout = Timeout,
    CooldownTimeout = Timeout,
    ProcessingTimeout = Timeout
};

} // namespace infra

namespace device_reminder {
using MessageType = infra::MessageType;
using ThreadMessageType = MessageType;
} // namespace device_reminder
