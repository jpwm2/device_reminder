#include "process_message_operation/process_message.hpp"
#include <memory>
#include <string>
#include <utility>

namespace device_reminder {

std::shared_ptr<IProcessMessage> ProcessMessage::clone() const {
    return std::make_shared<ProcessMessage>(*this);
}

std::string ProcessMessage::to_string() const {
    return "ProcessMessage{" + std::to_string(static_cast<int>(type_)) + "," +
           (payload_ ? "true" : "false") + "}";
}

} // namespace device_reminder
