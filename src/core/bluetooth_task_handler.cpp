#include "bluetooth_task/bluetooth_task_handler.hpp"
#include "message/message.hpp"

namespace device_reminder {
namespace {
Message parse_message(const std::string& s) {
    if (s == "scan") return Message{MessageType::BluetoothScanRequest};
    return Message{};
}
} // namespace

void BluetoothTaskHandler::handle(const std::string& msg_str) {
    if (task_) task_->run(parse_message(msg_str));
}

} // namespace device_reminder
