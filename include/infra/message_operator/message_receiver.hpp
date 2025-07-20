#pragma once

#include "message_operator/i_message_receiver.hpp"
#include "message_operator/i_message_queue.hpp"   // forward declaration of the queue interface
#include "message/message.hpp"

#include <mqueue.h>
#include <atomic>
#include <memory>
#include <string>

namespace device_reminder {

/**
 * @brief Concrete POSIX‑mqueue implementation of IMessageReceiver.
 *        It blocks on a named POSIX message queue and pushes received
 *        Message objects to the provided IMessageQueue.
 */
class MessageReceiver : public IMessageReceiver {
public:
    MessageReceiver(const std::string &mq_name,
                    std::shared_ptr<IMessageQueue> queue);
    ~MessageReceiver() override;

    void operator()() override;
    void stop() override;
    bool running() const noexcept override { return running_; }

private:
    static constexpr long kMsgSize = MESSAGE_SIZE;
    static constexpr long kMaxMsgs = 10;

    mqd_t open_queue(const std::string &name);
    void  close_queue();

    mqd_t                                      mq_{};
    std::string                                mq_name_;
    std::shared_ptr<IMessageQueue>             queue_;
    std::atomic<bool>                          running_{true};
};

} // namespace device_reminder
