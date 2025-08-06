#include "infra/message/message_codec/message_codec.hpp"
#include "infra/message/message.hpp"

#include <stdexcept>

namespace device_reminder {

namespace {

void append_uint32(std::vector<uint8_t>& buf, uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        buf.push_back(static_cast<uint8_t>((v >> (i * 8)) & 0xFF));
    }
}

uint32_t read_uint32(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos + 4 > buf.size()) {
        throw std::runtime_error("decode overflow");
    }
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
        v |= static_cast<uint32_t>(buf[pos++]) << (i * 8);
    }
    return v;
}

} // namespace

MessageCodec::MessageCodec(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

std::vector<uint8_t> MessageCodec::encode(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) {
            logger_->error("message is null");
        }
        throw std::invalid_argument("message is null");
    }

    std::vector<uint8_t> out;
    out.push_back(static_cast<uint8_t>(msg->type()));
    for (const auto& s : msg->payload()) {
        append_uint32(out, static_cast<uint32_t>(s.size()));
        out.insert(out.end(), s.begin(), s.end());
    }
    if (logger_) {
        logger_->info("encode success");
    }
    return out;
}

std::shared_ptr<IMessage> MessageCodec::decode(const std::vector<uint8_t>& data) {
    if (data.size() < 1) {
        if (logger_) {
            logger_->error("data too short");
        }
        throw std::invalid_argument("data too short");
    }

    size_t pos = 0;
    auto type = static_cast<MessageType>(data[pos++]);
    std::vector<std::string> payload;
    while (pos < data.size()) {
        uint32_t len = read_uint32(data, pos);
        if (pos + len > data.size()) {
            if (logger_) {
                logger_->error("data too short");
            }
            throw std::runtime_error("data too short");
        }
        payload.emplace_back(reinterpret_cast<const char*>(&data[pos]), len);
        pos += len;
    }

    auto msg = std::make_shared<Message>(type, payload, logger_);
    if (logger_) {
        logger_->info("decode success");
    }
    return msg;
}

} // namespace device_reminder
