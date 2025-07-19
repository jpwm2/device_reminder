#pragma once

#include "message_operator/i_message_sender.hpp"
#include "message_operator/message_queue.hpp"

#include <memory>
#include <string>

namespace device_reminder {

class MessageSender : public IMessageSender {
public:
    /// @brief POSIX メッセージキューへの送信ラッパー
    explicit MessageSender(std::shared_ptr<MessageQueue> queue);
    ~MessageSender() override = default;

    /// IMessageSender
    bool enqueue(uint32_t msg) override;
    void stop() override;

private:
    std::shared_ptr<MessageQueue> queue_;
};

} // namespace device_reminder
