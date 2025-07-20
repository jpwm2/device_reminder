#include "buzzer_task/buzzer_handler.hpp"
#include "message/message.hpp"

namespace device_reminder {
namespace {

Message parse_message(const std::string& s) {
    if (s == "on") return Message{MessageType::BuzzerOn};
    if (s == "off") return Message{MessageType::BuzzerOff};
    if (s == "timeout") return Message{MessageType::Timeout};
    return Message{};
}

} // namespace

void BuzzerHandler::handle(const std::string& msg_str) {
    if (task_) task_->send_message(parse_message(msg_str));
}

} // namespace device_reminder
