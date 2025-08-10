#pragma once

#include "infra/message/message_type.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>
#include <string>
#include <vector>

namespace device_reminder {

class IMessage {
public:
    virtual ~IMessage() = default;
    virtual MessageType type() const = 0;
    virtual std::vector<std::string> payload() const = 0;
    virtual std::string to_string() const = 0;
};

class Message : public IMessage {
public:
    Message(MessageType type, std::vector<std::string> payload);

    MessageType type() const override;
    std::vector<std::string> payload() const override;
    std::string to_string() const override;

private:
    MessageType type_;
    std::vector<std::string> payload_;
    std::shared_ptr<ILogger> logger_{};
};

} // namespace device_reminder

