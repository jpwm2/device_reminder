#pragma once
#include "infra/thread_message_operation/i_thread_message.hpp"
#include <cstddef>

namespace device_reminder {

struct ThreadMessage final : public IThreadMessage {
    constexpr ThreadMessage(MessageType t = MessageType::None,
                            bool p = false) noexcept
        : type_{t}, payload_{p} {}

    MessageType type() const noexcept override { return type_; }
    bool payload() const noexcept override { return payload_; }

    MessageType type_;
    bool payload_;
} __attribute__((packed));

constexpr std::size_t THREAD_MESSAGE_SIZE = sizeof(ThreadMessage);

static_assert(THREAD_MESSAGE_SIZE == sizeof(ThreadMessage),
              "Unexpected ThreadMessage size");

} // namespace device_reminder
