#pragma once
#include "infra/process_operation/message_codec/i_message_codec.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class MessageCodec : public IMessageCodec {
public:
    explicit MessageCodec(std::shared_ptr<ILogger> logger);

    std::vector<uint8_t> encode(std::shared_ptr<IProcessMessage> msg) override;
    std::shared_ptr<IProcessMessage> decode(const std::vector<uint8_t>& data) override;

private:
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
