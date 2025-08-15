#pragma once

#include "infra/logger.hpp"
#include "infra/message/message.hpp"
#include "infra/message/message_codec.hpp"

#include <memory>
#include <string>

namespace device_reminder {

class IProcessSender {
public:
    IProcessSender(std::shared_ptr<ILogger> logger,
                   std::shared_ptr<IMessageCodec> codec);
    virtual ~IProcessSender() = default;
    virtual void send(const std::string& endpoint,
                      std::shared_ptr<IMessage> message) = 0;

protected:
    std::shared_ptr<ILogger> logger_{};
    std::shared_ptr<IMessageCodec> codec_{};
};

class ProcessSender : public IProcessSender {
public:
    ProcessSender(std::shared_ptr<ILogger> logger,
                  std::shared_ptr<IMessageCodec> codec);

    void send(const std::string& endpoint,
              std::shared_ptr<IMessage> message) override;
};

} // namespace device_reminder

