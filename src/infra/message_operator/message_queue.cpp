#include "message_operator/message_queue.hpp"
#include "message/message.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <system_error>
#include <unistd.h>

namespace device_reminder {

namespace {

// 共通エラー変換
[[noreturn]] void throw_errno(const char* api) {
    throw std::system_error(errno, std::generic_category(),
                            std::string(api) + " failed: " + std::strerror(errno));
}

} // namespace

MessageQueue::MessageQueue(const std::string& name,
                           bool               create,
                           size_t             max_messages)
    : name_{name}
    , owner_{create}
{
    if (name_.empty() || name_[0] != '/')
        throw std::invalid_argument("POSIX mq name must start with '/'");

    if (create) {
        ::mq_attr attr{};
        attr.mq_flags   = 0;                       // ブロッキング
        attr.mq_maxmsg  = static_cast<long>(max_messages);
        attr.mq_msgsize = static_cast<long>(MESSAGE_SIZE);
        attr.mq_curmsgs = 0;

        mq_ = ::mq_open(name_.c_str(),
                        O_CREAT | O_RDWR,
                        0600,
                        &attr);
    } else {
        mq_ = ::mq_open(name_.c_str(), O_RDWR);
    }

    if (mq_ == static_cast<mqd_t>(-1))
        throw_errno("mq_open");
}

MessageQueue::~MessageQueue() {
    try   { close(); }
    catch (...) { /* 例外は投げない */ }
}

bool MessageQueue::is_open() const noexcept {
    return mq_ != static_cast<mqd_t>(-1);
}

void MessageQueue::close() {
    std::lock_guard lk{mtx_};
    if (!is_open()) return;

    ::mq_close(mq_);
    if (owner_) { ::mq_unlink(name_.c_str()); }
    mq_ = static_cast<mqd_t>(-1);
}

bool MessageQueue::push(const Message& msg) {
    std::lock_guard lk{mtx_};
    if (!is_open()) return false;

    if (::mq_send(mq_, reinterpret_cast<const char*>(&msg),
                  MESSAGE_SIZE, 0) == -1) {
        if (errno == EINTR) return push(msg);   // retry
        throw_errno("mq_send");
    }
    return true;
}

std::optional<Message> MessageQueue::pop() {
    Message out{};
    if (pop(out)) return out;
    return std::nullopt;
}

bool MessageQueue::pop(Message& out) {
    std::lock_guard lk{mtx_};
    if (!is_open()) return false;

    ssize_t n = ::mq_receive(mq_,
                             reinterpret_cast<char*>(&out),
                             MESSAGE_SIZE, nullptr);

    if (n == -1) {
        if (errno == EINTR) return pop(out);    // retry
        throw_errno("mq_receive");
    }
    return true;
}

} // namespace device_reminder
