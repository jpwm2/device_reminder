#pragma once
#include "infra/process_operation/process_message/i_process_message.hpp"
#include <memory>
#include <string>
#include <vector>

namespace device_reminder {

class ProcessMessage final : public IProcessMessage {
public:
    ProcessMessage(ProcessMessageType type, const std::vector<std::string>& payload);

    ProcessMessageType type() const noexcept override { return type_; }
    std::vector<std::string> payload() const noexcept override { return payload_; }
    std::shared_ptr<IProcessMessage> clone() const override;
    std::string to_string() const override;

private:
    ProcessMessageType type_;
    std::vector<std::string> payload_;
};

} // namespace device_reminder
