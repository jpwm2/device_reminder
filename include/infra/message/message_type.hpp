#pragma once

namespace device_reminder {

// スレッド間で送受信されるメッセージ種別を表す
enum class ThreadMessageType {
    None = 0,
    Unknown = None,
    HumanDetected,
    StartHumanDetection,
    StopHumanDetection,
    RequestBluetoothScan,
    RespondDeviceFound,
    RespondDeviceNotFound,
    StartBuzzing,
    StopBuzzing,
    BuzzTimeout,
    CooldownTimeout,
    ProcessingTimeout,

    // 既存実装との互換用エイリアス
    StartBuzzer = StartBuzzing,
    StopBuzzer = StopBuzzing,
    BluetoothScanRequested = RequestBluetoothScan,
    BluetoothScanResponse = RespondDeviceFound,
    HumanDetectStart = StartHumanDetection,
    HumanDetectStop = StopHumanDetection,
    Timeout = ProcessingTimeout
};

} // namespace device_reminder
