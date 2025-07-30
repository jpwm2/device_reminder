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
    CoolDownTimeout,
    ScanTimeout
};

} // namespace device_reminder
