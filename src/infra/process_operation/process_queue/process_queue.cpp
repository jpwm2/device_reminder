#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/logger/i_logger.hpp"

#include <stdexcept>
#include <utility>

namespace device_reminder {

ProcessQueue::ProcessQueue(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IMessageCodec> codec)
    : codec_{std::move(codec)}, logger_{std::move(logger)} {}

void ProcessQueue::push(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) logger_->warn("push called with nullptr message");
        return;
    }
    try {
        auto data = codec_->encode(msg);
        {
            std::lock_guard<std::mutex> lock{mtx_};
            buffer_.push(std::move(data));
        }
        if (logger_) logger_->info("message enqueued");
    } catch (const std::exception &e) {
        if (logger_) logger_->error(std::string{"failed to enqueue message: "} + e.what());
        throw;
    }
}

std::shared_ptr<IMessage> ProcessQueue::pop() {
    std::vector<uint8_t> data;
    {
        std::lock_guard<std::mutex> lock{mtx_};
        if (buffer_.empty()) {
            if (logger_) logger_->error("pop from empty queue");
            return nullptr;
        }
        data = std::move(buffer_.front());
        buffer_.pop();
    }
    try {
        auto msg = codec_->decode(data);
        if (!msg) {
            if (logger_) logger_->warn("decoded message is nullptr");
            return nullptr;
        }
        if (logger_) logger_->info("message dequeued");
        return msg;
    } catch (const std::exception &e) {
        if (logger_) logger_->error(std::string{"failed to decode message: "} + e.what());
        throw;
    }
}

} // namespace device_reminder

