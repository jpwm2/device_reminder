#include "infra/message/process_sender.hpp"

#include "infra/logger.hpp"
#include "infra/message/message_codec.hpp"
#include "infra/message/message_queue.hpp"

#include <stdexcept>
#include <utility>

namespace device_reminder {

ProcessSender::ProcessSender(std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IMessageCodec> codec)
    : logger_(std::move(logger)), codec_(std::move(codec)) {}

void ProcessSender::send(std::shared_ptr<IMessageQueue> queue,
                         std::shared_ptr<IMessage> msg) {
    if (!queue) {
        if (logger_) logger_->error("send: queue is null");
        throw std::invalid_argument("queue is null");
    }
    if (!msg) {
        if (logger_) logger_->error("send: msg is null");
        throw std::invalid_argument("msg is null");
    }

    if (logger_) {
        logger_->info("send: start, type=" +
                      std::to_string(static_cast<int>(msg->type())) +
                      ", payload_size=" +
                      std::to_string(msg->payload().size()));
    }

    try {
        if (!codec_) {
            if (logger_) logger_->error("send: codec is null");
            throw std::runtime_error("codec is null");
        }
        auto data = codec_->encode(msg);
        if (data.empty()) {
            if (logger_) logger_->warn("send: encoded data is empty");
        }

        queue->push(msg);
    } catch (const std::exception &e) {
        if (logger_) {
            logger_->error("send failed: " + std::string(e.what()));
        }
        throw;
    } catch (...) {
        if (logger_) {
            logger_->error("send failed: unknown exception");
        }
        throw;
    }

    if (logger_) {
        logger_->info("send: succeeded");
    }
}

} // namespace device_reminder

