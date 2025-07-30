#include "infra/process_operation/message_codec/message_codec.hpp"
#include "infra/process_operation/process_message/process_message.hpp"

#include <cstring>

namespace device_reminder {

MessageCodec::MessageCodec(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

namespace {

static void append_uint32(std::vector<uint8_t>& buf, uint32_t v) {
    for (int i = 0; i < 4; ++i) buf.push_back(static_cast<uint8_t>((v >> (i*8)) & 0xFF));
}

static uint32_t read_uint32(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos + 4 > buf.size()) throw std::runtime_error("decode overflow");
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) v |= static_cast<uint32_t>(buf[pos++]) << (i*8);
    return v;
}

}

std::vector<uint8_t> MessageCodec::encode(std::shared_ptr<IProcessMessage> msg) {
    std::vector<uint8_t> out;
    if (!msg) return out;
    out.push_back(static_cast<uint8_t>(msg->type()));
    const auto payload = msg->payload();
    append_uint32(out, static_cast<uint32_t>(payload.size()));
    for (const auto& s : payload) {
        append_uint32(out, static_cast<uint32_t>(s.size()));
        out.insert(out.end(), s.begin(), s.end());
    }
    return out;
}

std::shared_ptr<IProcessMessage> MessageCodec::decode(const std::vector<uint8_t>& data) {
    try {
        size_t pos = 0;
        if (data.size() < 1) throw std::runtime_error("data too short");
        auto type = static_cast<ProcessMessageType>(data[pos++]);
        uint32_t count = read_uint32(data, pos);
        std::vector<std::string> payload;
        payload.reserve(count);
        for (uint32_t i = 0; i < count; ++i) {
            uint32_t len = read_uint32(data, pos);
            if (pos + len > data.size()) throw std::runtime_error("data too short");
            payload.emplace_back(reinterpret_cast<const char*>(&data[pos]), len);
            pos += len;
        }
        return std::make_shared<ProcessMessage>(type, payload);
    } catch (const std::exception& e) {
        if (logger_) logger_->error(e.what());
        return nullptr;
    }
}

} // namespace device_reminder
