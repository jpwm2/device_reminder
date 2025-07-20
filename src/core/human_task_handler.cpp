#include "human_task/human_task_handler.hpp"
#include "message/message.hpp"

namespace device_reminder {

namespace {
Message parse_message(const std::string& s) {
    if (s == "start") return Message{MessageType::HumanDetectStart};
    if (s == "stop")  return Message{MessageType::HumanDetectStop};
    if (s == "detect") return Message{MessageType::HumanDetected};
    if (s == "timeout") return Message{MessageType::Timeout};
    return Message{};
}
} // namespace

void HumanTaskHandler::handle(const std::string& msg_str) {
    if (task_) task_->run(parse_message(msg_str));
}

} // namespace device_reminder

