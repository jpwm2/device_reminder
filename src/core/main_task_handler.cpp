#include "main_task/main_task_handler.hpp"
#include "message/message.hpp"

namespace device_reminder {
namespace {
Message parse_message(const std::string& s) {
    if (s == "human")     return Message{MessageType::HumanDetected};
    if (s == "device1")   return Message{MessageType::DeviceScanResult, true};
    if (s == "device0")   return Message{MessageType::DeviceScanResult, false};
    if (s == "t1")        return Message{MessageType::Timeout, true};
    if (s == "t0")        return Message{MessageType::Timeout, false};
    return Message{};
}
} // namespace

void MainTaskHandler::handle(const std::string& msg_str) {
    if (task_) task_->run(parse_message(msg_str));
}

} // namespace device_reminder
