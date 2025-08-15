#pragma once

#include "infra/logger.hpp"
#include "infra/message/message_codec.hpp"
#include "infra/message/thread_sender.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace device_reminder {

class IMessageInbox {
public:
    virtual ~IMessageInbox() = default;
    virtual void on_recv(const std::vector<uint8_t>& bytes) = 0;

protected:
    IMessageInbox(std::shared_ptr<ILogger> logger,
                  std::shared_ptr<IMessageCodec> codec,
                  std::shared_ptr<IThreadSender> sender,
                  std::shared_ptr<IMessageQueue> queue);

    std::shared_ptr<ILogger> logger_{};
    std::shared_ptr<IMessageCodec> codec_{};
    std::shared_ptr<IThreadSender> sender_{};
    std::shared_ptr<IMessageQueue> queue_{};
};

class MessageInbox : public IMessageInbox {
public:
    MessageInbox(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IMessageCodec> codec,
                 std::shared_ptr<IThreadSender> sender,
                 std::shared_ptr<IMessageQueue> queue);

    void on_recv(const std::vector<uint8_t>& bytes) override;
};

} // namespace device_reminder

