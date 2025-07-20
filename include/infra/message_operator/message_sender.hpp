#pragma once

#include "message_operator/i_message_sender.hpp"
#include "message_operator/message_queue.hpp"
#include "message/message.hpp"

#include <string>

namespace device_reminder {

class MessageSender : public IMessageSender {
public:
    /// @param queue_name   POSIX メッセージキュー名（例: "/device_reminder_mq"）
    /// @param max_messages キューの最大蓄積メッセージ数（mq_attr::mq_maxmsg）
    explicit MessageSender(const std::string& queue_name,
                           long               max_messages = 10);
    ~MessageSender() override;

    /// IMessageSender
    bool enqueue(const Message& msg) override;
    void stop() override;

private:
    std::string     queue_name_;
    MessageQueue    mq_;
};

} // namespace device_reminder
