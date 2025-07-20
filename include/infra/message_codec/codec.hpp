#pragma once
#include "message/message.hpp"
#include <array>
#include <memory>
#include <cstring>

namespace device_reminder {

using RawMsg = std::array<char, MESSAGE_SIZE>;

inline RawMsg encode(const IMessage& m) {
    RawMsg raw{};
    auto* out = reinterpret_cast<Message*>(raw.data());
    out->type_    = m.type();
    out->payload_ = m.payload();
    return raw;
}

inline std::unique_ptr<IMessage> decode(const char* buf, size_t sz) {
    if (sz != MESSAGE_SIZE) return {};
    const auto* in = reinterpret_cast<const Message*>(buf);
    return std::make_unique<Message>(in->type_, in->payload_);
}

} // namespace device_reminder
