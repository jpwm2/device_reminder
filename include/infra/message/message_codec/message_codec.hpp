#pragma once
#include "infra/message/message_codec/i_message_codec.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class MessageCodec : public IMessageCodec {
public:
    explicit MessageCodec(std::shared_ptr<ILogger> logger);

    std::vector<uint8_t> encode(std::shared_ptr<IMessage> msg) override;
    std::shared_ptr<IMessage> decode(const std::vector<uint8_t>& data) override;

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
