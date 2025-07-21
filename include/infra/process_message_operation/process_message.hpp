#pragma once
#include "infra/i_message.hpp"
#include <cstddef>

namespace device_reminder {

struct ProcessMessage final : public IMessage {
    constexpr ProcessMessage(MessageType t = MessageType::None,
                             bool p = false) noexcept
        : type_{t}, payload_{p} {}

    MessageType type() const noexcept override { return type_; }
    bool payload() const noexcept override { return payload_; }

    MessageType type_;
    bool payload_;
} __attribute__((packed));

constexpr std::size_t PROCESS_MESSAGE_SIZE = sizeof(ProcessMessage);

static_assert(PROCESS_MESSAGE_SIZE == sizeof(ProcessMessage),
              "Unexpected ProcessMessage size");

} // namespace device_reminder
