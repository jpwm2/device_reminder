#pragma once
// MessageCodec は IMessage をバイト列へエンコード／デコードする責務を持つ

#include <cstdint>
#include <memory>
#include <vector>

#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"

namespace device_reminder {

class IMessageCodec {
public:
    virtual ~IMessageCodec() = default;
    virtual std::vector<uint8_t> encode(std::shared_ptr<IMessage> msg) = 0;
    virtual std::shared_ptr<IMessage> decode(const std::vector<uint8_t>& data) = 0;
};

class MessageCodec : public IMessageCodec {
public:
    explicit MessageCodec(std::shared_ptr<ILogger> logger);

    std::vector<uint8_t> encode(std::shared_ptr<IMessage> msg) override;
    std::shared_ptr<IMessage> decode(const std::vector<uint8_t>& data) override;

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder

