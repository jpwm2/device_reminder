#pragma once

#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"
#include "infra/message/message_codec.hpp"
#include "infra/message/message_queue.hpp"

#include <memory>

namespace device_reminder {

class IProcessSender {
public:
    virtual ~IProcessSender() = default;
    virtual void send(std::shared_ptr<IMessageQueue> queue,
                      std::shared_ptr<IMessage> msg) = 0;
};

class ProcessSender : public IProcessSender {
public:
    ProcessSender(std::shared_ptr<ILogger> logger,
                  std::shared_ptr<IMessageCodec> codec);

    void send(std::shared_ptr<IMessageQueue> queue,
              std::shared_ptr<IMessage> msg) override;

private:
    std::shared_ptr<ILogger> logger_{};
    std::shared_ptr<IMessageCodec> codec_{};
};

} // namespace device_reminder

