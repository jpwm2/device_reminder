#pragma once

namespace device_reminder {

enum class ProcessMessageType {
    HumanDetected,
    StartHumanDetection,
    StopHumanDetection,
    RequestBluetoothScan,
    ResponseDevicePresence,
    StartBuzzing,
    StopBuzzing,
    BuzzTimeout,
    CooldownTimeout,
    ScanTimeout
};

} // namespace device_reminder
