#pragma once
#include "infra/thread_message_operation/i_thread_message.hpp"
#include <cstddef>
#include <memory>
#include <string>

namespace device_reminder {

struct ThreadMessage final : public IThreadMessage {
    constexpr ThreadMessage(ThreadMessageType t = ThreadMessageType::None,
                            bool p = false) noexcept
        : type_{t}, payload_{p} {}

    ThreadMessageType type() const noexcept override { return type_; }
    bool payload() const noexcept override { return payload_; }
    std::shared_ptr<IThreadMessage> clone() const override {
        return std::make_shared<ThreadMessage>(*this);
    }
    std::string to_string() const override {
        return "ThreadMessage{" + std::to_string(static_cast<int>(type_)) +
               "," + (payload_ ? "true" : "false") + "}";
    }

    ThreadMessageType type_;
    bool payload_;
} __attribute__((packed));

constexpr std::size_t THREAD_MESSAGE_SIZE = sizeof(ThreadMessage);

static_assert(THREAD_MESSAGE_SIZE == sizeof(ThreadMessage),
              "Unexpected ThreadMessage size");

} // namespace device_reminder
