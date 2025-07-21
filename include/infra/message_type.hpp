#pragma once

namespace device_reminder {

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

} // namespace device_reminder
