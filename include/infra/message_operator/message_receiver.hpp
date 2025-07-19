#pragma once

#include "message_operator/i_message_receiver.hpp"
#include "message_operator/i_message_queue.hpp"   // forward declaration of the queue interface

#include <mqueue.h>
#include <atomic>
#include <memory>
#include <string>

namespace device_reminder {

/**
 * @brief Concrete POSIX‑mqueue implementation of IMessageReceiver.
 *        It blocks on a named POSIX message queue and pushes received
 *        uint32_t messages to the provided thread‑safe IMessageQueue.
 */
class MessageReceiver : public IMessageReceiver {
public:
    MessageReceiver(const std::string &mq_name,
                    std::shared_ptr<IMessageQueue<uint32_t>> queue);
    ~MessageReceiver() override;

    void operator()() override;
    void stop() override;
    bool running() const noexcept override { return running_; }

private:
    static constexpr long kMsgSize = sizeof(uint32_t);
    static constexpr long kMaxMsgs = 10;

    mqd_t open_queue(const std::string &name);
    void  close_queue();

    mqd_t                                      mq_{};
    std::string                                mq_name_;
    std::shared_ptr<IMessageQueue<uint32_t>>    queue_;
    std::atomic<bool>                          running_{true};
};

} // namespace device_reminder
