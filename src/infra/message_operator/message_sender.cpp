#include "message_operator/message_sender.hpp"

namespace device_reminder {

MessageSender::MessageSender(std::shared_ptr<MessageQueue> queue)
    : queue_{std::move(queue)} {}

bool MessageSender::enqueue(uint32_t msg)
{
    if (!queue_) return false;
    return queue_->push(msg);
}

void MessageSender::stop()
{
    // no resources to release
}

} // namespace device_reminder
