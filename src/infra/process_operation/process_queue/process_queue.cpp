#include "infra/process_operation/process_queue/process_queue.hpp"

#include <utility>

namespace device_reminder {

ProcessQueue::ProcessQueue(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IMessageCodec> codec)
    : logger_{std::move(logger)}, codec_{std::move(codec)} {}

void ProcessQueue::push(std::shared_ptr<IMessage> msg) {
    if (!msg) {
        if (logger_) logger_->warn("push called with nullptr message");
        return;
    }

    try {
        auto data = codec_ ? codec_->encode(msg) : std::vector<uint8_t>{};
        {
            std::lock_guard lk{mtx_};
            buffer_.push(std::move(data));
        }
        if (logger_) logger_->info("enqueue success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("enqueue failed: ") + e.what());
        throw;
    }
}

std::shared_ptr<IMessage> ProcessQueue::pop() {
    std::vector<uint8_t> data;
    {
        std::lock_guard lk{mtx_};
        if (buffer_.empty()) {
            if (logger_) logger_->error("dequeue failed: queue empty");
            return nullptr;
        }
        data = std::move(buffer_.front());
        buffer_.pop();
    }

    try {
        auto msg = codec_ ? codec_->decode(data) : nullptr;
        if (!msg) {
            if (logger_) logger_->warn("decode returned nullptr");
            return nullptr;
        }
        if (logger_) logger_->info("dequeue success");
        return msg;
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("decode failed: ") + e.what());
        throw;
    }
}

std::size_t ProcessQueue::size() const {
    std::lock_guard lk{mtx_};
    return buffer_.size();
}

} // namespace device_reminder

