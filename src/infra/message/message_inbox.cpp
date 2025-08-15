#include "infra/message/message_inbox.hpp"

#include <stdexcept>
#include <utility>

namespace device_reminder {

IMessageInbox::IMessageInbox(std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IMessageCodec> codec,
                             std::shared_ptr<IThreadSender> sender,
                             std::shared_ptr<IMessageQueue> queue)
    : logger_(std::move(logger)),
      codec_(std::move(codec)),
      sender_(std::move(sender)),
      queue_(std::move(queue)) {}

MessageInbox::MessageInbox(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IMessageCodec> codec,
                           std::shared_ptr<IThreadSender> sender,
                           std::shared_ptr<IMessageQueue> queue)
    : IMessageInbox(std::move(logger), std::move(codec), std::move(sender), std::move(queue)) {}

void MessageInbox::on_recv(const std::vector<uint8_t>& bytes) {
    if (logger_) {
        logger_->info("MessageInbox on_recv start: size=" + std::to_string(bytes.size()));
    }
    try {
        if (!codec_) {
            if (logger_) logger_->error("MessageInbox on_recv failed: codec is null");
            throw std::runtime_error("codec is null");
        }
        if (!sender_) {
            if (logger_) logger_->error("MessageInbox on_recv failed: sender is null");
            throw std::runtime_error("sender is null");
        }
        if (!queue_) {
            if (logger_) logger_->error("MessageInbox on_recv failed: queue is null");
            throw std::runtime_error("queue is null");
        }

        auto msg = codec_->decode(bytes);
        if (logger_ && msg) {
            logger_->info("MessageInbox decode success: " + msg->to_string());
        }

        sender_->send(queue_, msg);

        if (logger_) {
            logger_->info("MessageInbox on_recv success");
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error(std::string("MessageInbox on_recv exception: ") + e.what());
        }
        throw;
    } catch (...) {
        if (logger_) {
            logger_->error("MessageInbox on_recv unknown exception");
        }
        throw;
    }
}

} // namespace device_reminder

