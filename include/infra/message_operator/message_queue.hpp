#pragma once
#include "i_message_queue.hpp"
#include "message/message.hpp"

#include <mqueue.h>
#include <mutex>
#include <string>

namespace device_reminder {

class MessageQueue final : public IMessageQueue {
public:
    /**
     * @param name  POSIX メッセージキュー名（例: "/devreminder_mq"）。先頭に '/' が必須
     * @param create true なら O_CREAT で生成し、false なら既存を開く
     * @param max_messages  キュー長（create==true のとき有効）
     */
    explicit MessageQueue(const std::string& name,
                          bool   create        = false,
                          size_t max_messages  = 16);

    ~MessageQueue() override;

    bool push(const Message& msg) override;
    std::optional<Message> pop() override;
    bool pop(Message& out) override;

    bool is_open() const noexcept override;
    void close() override;

private:
    mqd_t       mq_{static_cast<mqd_t>(-1)};
    std::string name_;
    std::mutex  mtx_;
    bool        owner_{false};   ///< create==true だった場合に true
};

} // namespace device_reminder
