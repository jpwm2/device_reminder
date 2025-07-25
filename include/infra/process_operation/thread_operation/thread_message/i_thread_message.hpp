#pragma once
#include <cstddef>

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

class IThreadMessage {
public:
    virtual ~IThreadMessage() = default;
    virtual MessageType type() const noexcept = 0;
    virtual bool payload() const noexcept = 0;
};

} // namespace device_reminder
