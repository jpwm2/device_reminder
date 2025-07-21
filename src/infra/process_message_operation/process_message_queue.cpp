#include "process_message_operation/process_message_queue.hpp"
#include "infra/logger/i_logger.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <system_error>
#include <optional>
#include <unistd.h>

namespace device_reminder {

namespace {
[[noreturn]] void throw_errno(const char* api) {
    throw std::system_error(errno, std::generic_category(),
                            std::string(api) + " failed: " + std::strerror(errno));
}
} // namespace

ProcessMessageQueue::ProcessMessageQueue(const std::string& name,
                                         bool create,
                                         size_t max_messages,
                                         std::shared_ptr<ILogger> logger)
    : name_{name}
    , owner_{create}
    , logger_(std::move(logger))
{
    if (logger_) logger_->info("ProcessMessageQueue created: " + name_);
    if (name_.empty() || name_[0] != '/')
        throw std::invalid_argument("POSIX mq name must start with '/'");

    if (create) {
        ::mq_attr attr{};
        attr.mq_flags   = 0;
        attr.mq_maxmsg  = static_cast<long>(max_messages);
        attr.mq_msgsize = static_cast<long>(PROCESS_MESSAGE_SIZE);
        attr.mq_curmsgs = 0;

        mq_ = ::mq_open(name_.c_str(),
                        O_CREAT | O_RDWR,
                        0600,
                        &attr);
    } else {
        mq_ = ::mq_open(name_.c_str(), O_RDWR);
    }

    if (mq_ == static_cast<mqd_t>(-1)) {
        if (logger_) logger_->error("mq_open failed");
        throw_errno("mq_open");
    }
}

ProcessMessageQueue::~ProcessMessageQueue() {
    try { close(); } catch (...) {}
    if (logger_) logger_->info("ProcessMessageQueue destroyed");
}

bool ProcessMessageQueue::is_open() const noexcept {
    return mq_ != static_cast<mqd_t>(-1);
}

void ProcessMessageQueue::close() {
    std::lock_guard lk{mtx_};
    if (!is_open()) return;

    ::mq_close(mq_);
    if (owner_) { ::mq_unlink(name_.c_str()); }
    mq_ = static_cast<mqd_t>(-1);
    if (logger_) logger_->info("ProcessMessageQueue closed");
}

bool ProcessMessageQueue::push(const ProcessMessage& msg) {
    std::lock_guard lk{mtx_};
    if (!is_open()) return false;

    if (::mq_send(mq_, reinterpret_cast<const char*>(&msg),
                  PROCESS_MESSAGE_SIZE, 0) == -1) {
        if (errno == EINTR) return push(msg);
        if (logger_) logger_->error("mq_send failed");
        throw_errno("mq_send");
    }
    return true;
}

std::optional<ProcessMessage> ProcessMessageQueue::pop() {
    ProcessMessage out{};
    if (pop(out)) return out;
    return std::nullopt;
}

bool ProcessMessageQueue::pop(ProcessMessage& out) {
    std::lock_guard lk{mtx_};
    if (!is_open()) return false;

    ssize_t n = ::mq_receive(mq_,
                             reinterpret_cast<char*>(&out),
                             PROCESS_MESSAGE_SIZE, nullptr);

    if (n == -1) {
        if (errno == EINTR) return pop(out);
        if (logger_) logger_->error("mq_receive failed");
        throw_errno("mq_receive");
    }
    return true;
}

} // namespace device_reminder
