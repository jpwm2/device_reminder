#pragma once
#include <memory>
#include <string>

namespace device_reminder {

enum class ThreadMessageType {
    HumanDetected,
    StartBuzzer,
    StopBuzzer,
    Timeout,
    BluetoothScanRequested,
    BluetoothScanResponse
};

class IThreadMessage {
public:
    virtual ~IThreadMessage() = default;
    virtual ThreadMessageType type() const noexcept = 0;
    virtual std::vector<std::string> payload() const = 0;
    virtual std::shared_ptr<IThreadMessage> clone() const = 0;
    virtual std::string to_string() const = 0;
};

} // namespace device_reminder
