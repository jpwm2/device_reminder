#pragma once

namespace infra {

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
    Timeout
};

} // namespace infra
