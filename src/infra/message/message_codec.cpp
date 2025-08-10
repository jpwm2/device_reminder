#include "infra/message/message_codec.hpp"
#include "infra/message/message.hpp"

#include <cstring>
#include <stdexcept>

namespace device_reminder {

namespace {

constexpr uint8_t  kMagic[2]   = {0x4D, 0x43};
constexpr uint8_t  kVersion    = 0x01;
constexpr uint8_t  kFlags      = 0x00;
constexpr size_t   kHeaderSize = 2 + 1 + 1 + 2 + 4; // fixed header length
constexpr size_t   kFooterSize = 4;                 // CRC32 length
constexpr size_t   kMinFrame   = kHeaderSize + kFooterSize;
constexpr size_t   kMaxPayload = 1024 * 1024; // 任意の上限(1MB)

uint16_t read_u16(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos + 2 > buf.size()) throw std::runtime_error("decode overflow");
    uint16_t v = (static_cast<uint16_t>(buf[pos]) << 8) |
                 static_cast<uint16_t>(buf[pos + 1]);
    pos += 2;
    return v;
}

uint32_t read_u32(const std::vector<uint8_t>& buf, size_t& pos) {
    if (pos + 4 > buf.size()) throw std::runtime_error("decode overflow");
    uint32_t v = (static_cast<uint32_t>(buf[pos]) << 24) |
                 (static_cast<uint32_t>(buf[pos + 1]) << 16) |
                 (static_cast<uint32_t>(buf[pos + 2]) << 8) |
                 static_cast<uint32_t>(buf[pos + 3]);
    pos += 4;
    return v;
}

void append_u16(std::vector<uint8_t>& buf, uint16_t v) {
    buf.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    buf.push_back(static_cast<uint8_t>(v & 0xFF));
}

void append_u32(std::vector<uint8_t>& buf, uint32_t v) {
    buf.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    buf.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    buf.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    buf.push_back(static_cast<uint8_t>(v & 0xFF));
}

uint32_t crc32(const uint8_t* data, size_t len) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320u : (crc >> 1);
        }
    }
    return ~crc;
}

} // namespace

MessageCodec::MessageCodec(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger)) {}

std::vector<uint8_t> MessageCodec::encode(std::shared_ptr<IMessage> msg) {
    try {
        if (!msg) {
            throw std::invalid_argument("msg is null");
        }

        const auto payload = msg->payload();
        if (payload.size() > kMaxPayload) {
            throw std::range_error("payload too large");
        }

        if (logger_) {
            logger_->info("encode start");
        }

        std::vector<uint8_t> out;
        out.reserve(kHeaderSize + payload.size() + kFooterSize);

        out.push_back(kMagic[0]);
        out.push_back(kMagic[1]);
        out.push_back(kVersion);
        out.push_back(kFlags);
        append_u16(out, static_cast<uint16_t>(msg->type()));
        append_u32(out, static_cast<uint32_t>(payload.size()));
        out.insert(out.end(), payload.begin(), payload.end());

        uint32_t crc = crc32(out.data(), out.size());
        append_u32(out, crc);

        if (logger_) {
            logger_->info("encode success");
        }
        return out;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error(e.what());
        }
        throw;
    }
}

std::shared_ptr<IMessage> MessageCodec::decode(const std::vector<uint8_t>& data) {
    try {
        if (data.size() < kMinFrame) {
            throw std::invalid_argument("frame too short");
        }

        if (logger_) {
            logger_->info("decode start");
        }

        size_t pos = 0;
        if (data[pos++] != kMagic[0] || data[pos++] != kMagic[1]) {
            throw std::invalid_argument("invalid magic");
        }

        uint8_t version = data[pos++];
        if (version != kVersion) {
            throw std::runtime_error("unsupported version");
        }

        pos++; // flags

        auto type = static_cast<MessageType>(read_u16(data, pos));
        uint32_t length = read_u32(data, pos);

        if (data.size() < pos + length + kFooterSize) {
            throw std::invalid_argument("invalid length");
        }

        size_t body_pos = pos;
        pos += length;
        uint32_t recv_crc = read_u32(data, pos);
        uint32_t calc_crc = crc32(data.data(), body_pos + length);
        if (recv_crc != calc_crc) {
            throw std::runtime_error("crc mismatch");
        }

        std::vector<uint8_t> payload(data.begin() + body_pos, data.begin() + body_pos + length);
        auto msg = std::make_shared<Message>(type, std::move(payload));

        if (logger_) {
            logger_->info("decode success");
        }
        return msg;
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error(e.what());
        }
        throw;
    }
}

} // namespace device_reminder
