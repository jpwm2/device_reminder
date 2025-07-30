#pragma once
#include "infra/process_operation/process_message/i_process_message.hpp"
#include <cstddef>
#include <memory>
#include <string>

namespace device_reminder {

class ProcessMessage final : public IProcessMessage {
public:
    constexpr ProcessMessage(ProcessMessageType t, bool p = false) noexcept
        : type_{t}, payload_{p} {}

    ProcessMessageType type() const noexcept override { return type_; }
    bool payload() const noexcept override { return payload_; }
    std::shared_ptr<IProcessMessage> clone() const override;
    std::string to_string() const override;

    ProcessMessageType type_;
    bool payload_;
} __attribute__((packed));

constexpr std::size_t PROCESS_MESSAGE_SIZE = sizeof(ProcessMessage);

static_assert(PROCESS_MESSAGE_SIZE == sizeof(ProcessMessage),
              "Unexpected ProcessMessage size");

} // namespace device_reminder
