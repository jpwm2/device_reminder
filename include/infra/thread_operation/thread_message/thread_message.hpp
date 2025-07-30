#pragma once
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace device_reminder {

class ThreadMessage final : public IThreadMessage {
public:
    ThreadMessage(ThreadMessageType type, const std::vector<std::string>& payload)
        : type_{type}, payload_{payload} {}

    ThreadMessageType type() const noexcept override { return type_; }
    std::vector<std::string> payload() const override { return payload_; }
    std::shared_ptr<IThreadMessage> clone() const override {
        return std::make_shared<ThreadMessage>(*this);
    }
    std::string to_string() const override {
        std::string result = "ThreadMessage{" + std::to_string(static_cast<int>(type_)) + ",[";
        for (std::size_t i = 0; i < payload_.size(); ++i) {
            result += payload_[i];
            if (i + 1 < payload_.size()) result += ",";
        }
        result += "]}";
        return result;
    }

private:
    ThreadMessageType type_;
    std::vector<std::string> payload_;
};

} // namespace device_reminder
