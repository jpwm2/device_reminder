#pragma once
#include <cstddef>
#include <memory>
#include <string>

namespace device_reminder {

enum class ThreadMessageType {
    None,
    Timeout,
    HumanDetected,
    HumanDetectStart,
    HumanDetectStop,
    BluetoothEvent,
    StartBuzzer,
    StopBuzzer,
    BluetoothScanRequested,
    BluetoothScanResponse
};

class IThreadMessage {
public:
    virtual ~IThreadMessage() = default;
    virtual ThreadMessageType type() const noexcept = 0;
    virtual bool payload() const noexcept = 0;
    virtual std::shared_ptr<IThreadMessage> clone() const = 0;
    virtual std::string to_string() const = 0;
};

} // namespace device_reminder
