#include "infra/message/message.hpp"

#include <utility>
#include <sstream>

namespace device_reminder {

Message::Message(MessageType type,
                 std::vector<std::string> payload,
                 std::shared_ptr<ILogger> logger)
    : type_{type},
      payload_{std::move(payload)},
      logger_{std::move(logger)} {}

MessageType Message::type() const {
    if (logger_) logger_->info("type start");
    try {
        if (logger_) logger_->info("type success: " + std::to_string(static_cast<int>(type_)));
        return type_;
    } catch (const std::exception& e) {
        if (logger_) logger_->error(e.what());
        throw;
    }
}

std::vector<std::string> Message::payload() const {
    if (logger_) logger_->info("payload start");
    try {
        if (logger_) logger_->info("payload success: size=" + std::to_string(payload_.size()));
        return payload_;
    } catch (const std::exception& e) {
        if (logger_) logger_->error(e.what());
        throw;
    }
}

std::string Message::to_string() const {
    if (logger_) logger_->info("to_string start");
    try {
        std::ostringstream oss;
        oss << static_cast<int>(type_);
        for (const auto& s : payload_) {
            oss << "," << s;
        }
        auto result = oss.str();
        if (logger_) logger_->info("to_string success: " + result);
        return result;
    } catch (const std::exception& e) {
        if (logger_) logger_->error(e.what());
        throw;
    }
}

} // namespace device_reminder

