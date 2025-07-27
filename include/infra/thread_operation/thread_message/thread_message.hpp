#pragma once
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include <memory>
#include <string>
#include <utility>

namespace device_reminder {

class ThreadMessage final : public IThreadMessage {
public:
    ThreadMessage(ThreadMessageType type, std::string payload)
        : type_{type}, payload_{std::move(payload)} {}

    ThreadMessageType type() const noexcept override { return type_; }
    const std::string& payload() const noexcept { return payload_; }
    std::shared_ptr<IThreadMessage> clone() const override {
        return std::make_shared<ThreadMessage>(*this);
    }
    std::string to_string() const override {
        return "ThreadMessage{" + std::to_string(static_cast<int>(type_)) + "," + payload_ + "}";
    }

private:
    ThreadMessageType type_;
    std::string payload_;
};

} // namespace device_reminder
