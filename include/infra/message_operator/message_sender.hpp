#pragma once

#include "message_operator/i_message_sender.hpp"
#include "message_operator/thread_safe_queue.hpp"

#include <atomic>
#include <memory>
#include <mqueue.h>
#include <string>
#include <thread>

namespace device_reminder {

class MessageSender : public IMessageSender {
public:
    /// @param queue_name   POSIX メッセージキュー名（例: "/device_reminder_mq"）
    /// @param max_messages キューの最大蓄積メッセージ数（mq_attr::mq_maxmsg）
    explicit MessageSender(const std::string& queue_name,
                           long               max_messages = 10,
                           std::shared_ptr<TSQueue<uint32_t>> q =
                               std::make_shared<TSQueue<uint32_t>>());
    ~MessageSender() override;

    /// IMessageSender
    bool enqueue(uint32_t msg) override;
    void stop() override;

private:
    void run();  // スレッド本体

    std::string                     queue_name_;
    std::shared_ptr<TSQueue<uint32_t>> queue_;
    std::thread                     thread_;
    std::atomic<bool>               running_{true};
    mqd_t                           mq_{-1};
};

} // namespace device_reminder
