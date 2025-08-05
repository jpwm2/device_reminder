#include "infra/message/message.hpp"
#include <sstream>
#include <stdexcept>
#include <utility>

namespace device_reminder {

Message::Message(MessageType type,
                 std::vector<std::string> payload,
                 std::shared_ptr<ILogger> logger)
    : type_{type}, payload_{std::move(payload)}, logger_{std::move(logger)} {
    if (!logger_) {
        throw std::invalid_argument{"logger must not be null"};
    }
}

std::string Message::to_string() const {
    std::string type_str;
    switch (type_) {
    case MessageType::None: type_str = "None"; break;
    case MessageType::HumanFound: type_str = "HumanFound"; break;
    case MessageType::StartHumanTask: type_str = "StartHumanTask"; break;
    case MessageType::StopHumanTask: type_str = "StopHumanTask"; break;
    case MessageType::StartBluetoothTask: type_str = "StartBluetoothTask"; break;
    case MessageType::DeviceFound: type_str = "DeviceFound"; break;
    case MessageType::DeviceNotFound: type_str = "DeviceNotFound"; break;
    case MessageType::StartBuzzer: type_str = "StartBuzzer"; break;
    case MessageType::StopBuzzer: type_str = "StopBuzzer"; break;
    case MessageType::Timeout: type_str = "Timeout"; break;
    }

    std::ostringstream oss;
    oss << "Type: " << type_str << ", Payload: [";
    for (size_t i = 0; i < payload_.size(); ++i) {
        oss << payload_[i];
        if (i + 1 < payload_.size()) {
            oss << ", ";
        }
    }
    oss << "]";

    auto msg = oss.str();
    logger_->info(msg);
    return msg;
}

} // namespace device_reminder

