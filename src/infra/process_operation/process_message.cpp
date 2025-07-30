#include "process_operation/process_message/process_message.hpp"
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace device_reminder {

ProcessMessage::ProcessMessage(ProcessMessageType type,
                               const std::vector<std::string>& payload)
    : type_{type}, payload_{payload} {}

std::shared_ptr<IProcessMessage> ProcessMessage::clone() const {
    return std::make_shared<ProcessMessage>(*this);
}

std::string ProcessMessage::to_string() const {
    std::ostringstream oss;
    oss << "ProcessMessage{" << static_cast<int>(type_) << ",[";
    for (size_t i = 0; i < payload_.size(); ++i) {
        oss << payload_[i];
        if (i + 1 < payload_.size()) oss << ',';
    }
    oss << "]}";
    return oss.str();
}

} // namespace device_reminder
