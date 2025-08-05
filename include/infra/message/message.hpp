#pragma once
#include "infra/logger/i_logger.hpp"
#include "infra/message/i_message.hpp"
#include <memory>
#include <string>
#include <vector>

namespace device_reminder {

class Message final : public IMessage {
public:
    Message(MessageType type,
            std::vector<std::string> payload,
            std::shared_ptr<ILogger> logger);

    MessageType type() const noexcept override { return type_; }
    std::vector<std::string> payload() const noexcept override { return payload_; }
    std::string to_string() const override;

private:
    MessageType type_;
    std::vector<std::string> payload_;
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder

