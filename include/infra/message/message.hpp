#pragma once

#include "message/i_message.hpp"

namespace device_reminder {

class Message : public IMessage {
public:
    Message(MessageType type, bool payload)
        : type_(type), payload_(payload) {}

    MessageType type() const override {
        return type_;
    }

    bool payload() const override {
        return payload_;
    }

private:
    MessageType type_;
    bool payload_;
};

} // namespace device_reminder