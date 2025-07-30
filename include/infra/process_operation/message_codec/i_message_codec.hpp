#pragma once
#include "infra/process_operation/process_message/i_process_message.hpp"
#include <memory>
#include <vector>

namespace device_reminder {

class IMessageCodec {
public:
    virtual ~IMessageCodec() = default;
    virtual std::vector<uint8_t> encode(std::shared_ptr<IProcessMessage> msg) = 0;
    virtual std::shared_ptr<IProcessMessage> decode(const std::vector<uint8_t>& data) = 0;
};

} // namespace device_reminder
