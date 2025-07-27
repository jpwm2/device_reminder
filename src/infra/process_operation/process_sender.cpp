#include "process_message_operation/process_message_sender.hpp"
#include "infra/logger/i_logger.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <system_error>
#include <sys/stat.h>
#include <unistd.h>

namespace device_reminder {

ProcessMessageSender::ProcessMessageSender(const std::string& queue_name,
                                           long max_messages,
                                           std::shared_ptr<ILogger> logger)
    : queue_name_{queue_name},
      mq_{queue_name_, true, static_cast<size_t>(max_messages), logger},
      logger_(std::move(logger))
{
    if (logger_) logger_->info("ProcessMessageSender created: " + queue_name_);
}

ProcessMessageSender::~ProcessMessageSender() {
    stop();
    if (logger_) logger_->info("ProcessMessageSender destroyed");
}

bool ProcessMessageSender::enqueue(const ProcessMessage& msg) {
    if (logger_) logger_->info("ProcessMessageSender enqueue");
    return mq_.push(msg);
}

void ProcessMessageSender::stop() {
    mq_.close();
    if (logger_) logger_->info("ProcessMessageSender stop");
}

} // namespace device_reminder
