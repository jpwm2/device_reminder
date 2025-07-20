#include "message_operator/message_sender.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <system_error>
#include <sys/stat.h>
#include <unistd.h>

namespace device_reminder {
//--------------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------------
MessageSender::MessageSender(const std::string& queue_name,
                             long               max_messages)
    : queue_name_{queue_name},
      mq_{queue_name_, true, static_cast<size_t>(max_messages)}
{
}
//--------------------------------------------------------------------
MessageSender::~MessageSender()
{
    stop();
}
//--------------------------------------------------------------------
bool MessageSender::enqueue(const Message& msg)
{
    return mq_.push(msg);
}
//--------------------------------------------------------------------
void MessageSender::stop()
{
    mq_.close();
}


} // namespace device_reminder
