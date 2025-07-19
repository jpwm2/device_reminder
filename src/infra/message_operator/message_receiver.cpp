#include "message_operator/message_receiver.hpp"

#include <chrono>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/stat.h>
#include <thread>

namespace device_reminder {

mqd_t MessageReceiver::open_queue(const std::string &name) {
    struct mq_attr attr {};
    attr.mq_flags   = 0;            // blocking mode
    attr.mq_maxmsg  = kMaxMsgs;
    attr.mq_msgsize = kMsgSize;
    attr.mq_curmsgs = 0;

    int flags = O_RDONLY | O_CREAT;
    mqd_t mqd = mq_open(name.c_str(), flags, 0666, &attr);
    if (mqd == static_cast<mqd_t>(-1)) {
        throw std::runtime_error("mq_open failed: " + std::string(strerror(errno)));
    }
    return mqd;
}

void MessageReceiver::close_queue() {
    if (mq_ != static_cast<mqd_t>(-1)) {
        mq_close(mq_);
        mq_unlink(mq_name_.c_str());
        mq_ = static_cast<mqd_t>(-1);
    }
}

MessageReceiver::MessageReceiver(const std::string &mq_name,
                                 std::shared_ptr<IMessageQueue<uint32_t>> queue)
    : mq_name_(mq_name), queue_(std::move(queue)) {
    mq_ = open_queue(mq_name_);
}

MessageReceiver::~MessageReceiver() {
    stop();
    close_queue();
}

void MessageReceiver::stop() {
    if (!running_) return;
    running_ = false;

    // Wake the blocking mq_receive() by sending a dummy message.
    mqd_t sender = mq_open(mq_name_.c_str(), O_WRONLY);
    if (sender != static_cast<mqd_t>(-1)) {
        uint32_t dummy = 0;
        mq_send(sender, reinterpret_cast<const char *>(&dummy), kMsgSize, 0);
        mq_close(sender);
    }
}

void MessageReceiver::operator()() {
    uint32_t msg{};
    while (running_) {
        ssize_t n = mq_receive(mq_, reinterpret_cast<char *>(&msg), kMsgSize, nullptr);
        if (n >= 0) {
            if (!running_) break;                 // stopped via dummy message
            if (queue_) queue_->push(msg);
        } else if (errno == EINTR) {
            continue;                             // interrupted by a signal, retry
        } else {
            // Unexpected error – avoid busy looping
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

} // namespace device_reminder
