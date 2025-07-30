#include "infra/process_operation/process_queue/process_queue.hpp"
#include "infra/logger/i_logger.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <system_error>
#include <unistd.h>

namespace device_reminder {

namespace {
[[noreturn]] void throw_errno(const char* api) {
    throw std::system_error(errno, std::generic_category(),
                            std::string(api) + " failed: " + std::strerror(errno));
}
}

ProcessQueue::ProcessQueue(std::shared_ptr<ILogger> logger,
                           std::shared_ptr<IMessageCodec> codec,
                           const std::string& queue_name)
    : queue_name_{queue_name}, codec_{std::move(codec)}, logger_{std::move(logger)}
{
    if (logger_) logger_->info("ProcessQueue created: " + queue_name_);
    if (queue_name_.empty() || queue_name_[0] != '/')
        throw std::invalid_argument("POSIX mq name must start with '/'");

    ::mq_attr attr{};
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = kMaxMsg;
    attr.mq_msgsize = kMsgSize;
    attr.mq_curmsgs = 0;

    mq_ = ::mq_open(queue_name_.c_str(), O_CREAT | O_RDWR, 0600, &attr);
    if (mq_ == static_cast<mqd_t>(-1)) {
        if (logger_) logger_->error("mq_open failed");
        throw_errno("mq_open");
    }
}

ProcessQueue::~ProcessQueue() {
    std::lock_guard lk{mtx_};
    if (mq_ != static_cast<mqd_t>(-1)) {
        ::mq_close(mq_);
        ::mq_unlink(queue_name_.c_str());
        mq_ = static_cast<mqd_t>(-1);
    }
    if (logger_) logger_->info("ProcessQueue destroyed");
}

void ProcessQueue::push(std::shared_ptr<IProcessMessage> msg) {
    if (!msg || !codec_) return;
    auto data = codec_->encode(msg);
    std::lock_guard lk{mtx_};
    if (mq_ == static_cast<mqd_t>(-1)) return;
    if (::mq_send(mq_, reinterpret_cast<const char*>(data.data()), data.size(), 0) == -1) {
        if (errno == EINTR) { push(msg); return; }
        if (logger_) logger_->error("mq_send failed");
        throw_errno("mq_send");
    }
}

std::shared_ptr<IProcessMessage> ProcessQueue::pop() {
    std::vector<uint8_t> buf(kMsgSize);
    std::lock_guard lk{mtx_};
    if (mq_ == static_cast<mqd_t>(-1)) return nullptr;
    ssize_t n = ::mq_receive(mq_, reinterpret_cast<char*>(buf.data()), kMsgSize, nullptr);
    if (n == -1) {
        if (errno == EINTR) return pop();
        if (logger_) logger_->error("mq_receive failed");
        throw_errno("mq_receive");
    }
    buf.resize(static_cast<size_t>(n));
    if (!codec_) return nullptr;
    return codec_->decode(buf);
}

std::size_t ProcessQueue::size() const {
    struct mq_attr attr{};
    std::lock_guard lk{mtx_};
    if (mq_getattr(mq_, &attr) == -1) {
        if (logger_) logger_->error("mq_getattr failed");
        return 0;
    }
    return static_cast<std::size_t>(attr.mq_curmsgs);
}

} // namespace device_reminder
