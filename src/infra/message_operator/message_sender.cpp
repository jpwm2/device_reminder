#include "message_operator/message_sender.hpp"
#include "infra/logger/i_logger.hpp"

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
                             long               max_messages,
                             std::shared_ptr<ILogger> logger)
    : queue_name_{queue_name},
      mq_{queue_name_, true, static_cast<size_t>(max_messages), logger},
      logger_(std::move(logger))
{
    if (logger_) logger_->info("MessageSender created: " + queue_name_);
}
//--------------------------------------------------------------------
MessageSender::~MessageSender()
{
    stop();
    if (logger_) logger_->info("MessageSender destroyed");
}
//--------------------------------------------------------------------
bool MessageSender::enqueue(const Message& msg)
{
    if (logger_) logger_->info("MessageSender enqueue");
    return mq_.push(msg);
}
//--------------------------------------------------------------------
void MessageSender::stop()
{
    mq_.close();
    if (logger_) logger_->info("MessageSender stop");
}


} // namespace device_reminder
