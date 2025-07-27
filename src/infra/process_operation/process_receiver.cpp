#include "process_message_operation/process_message_receiver.hpp"
#include "thread_message_operation/thread_message.hpp"
#include "infra/logger/i_logger.hpp"

#include <chrono>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/stat.h>
#include <thread>

namespace device_reminder {

mqd_t ProcessMessageReceiver::open_queue(const std::string &name) {
    struct mq_attr attr {};
    attr.mq_flags   = 0;
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

void ProcessMessageReceiver::close_queue() {
    if (mq_ != static_cast<mqd_t>(-1)) {
        mq_close(mq_);
        mq_unlink(mq_name_.c_str());
        mq_ = static_cast<mqd_t>(-1);
    }
}

ProcessMessageReceiver::ProcessMessageReceiver(const std::string &mq_name,
                                               std::shared_ptr<IThreadMessageQueue> queue,
                                               std::shared_ptr<ILogger> logger)
    : mq_name_(mq_name), queue_(std::move(queue)), logger_(std::move(logger)) {
    mq_ = open_queue(mq_name_);
    if (logger_) logger_->info("ProcessMessageReceiver open: " + mq_name_);
}

ProcessMessageReceiver::~ProcessMessageReceiver() {
    stop();
    close_queue();
    if (logger_) logger_->info("ProcessMessageReceiver destroyed");
}

void ProcessMessageReceiver::stop() {
    if (!running_) return;
    running_ = false;

    mqd_t sender = mq_open(mq_name_.c_str(), O_WRONLY);
    if (sender != static_cast<mqd_t>(-1)) {
        ProcessMessage dummy{};
        mq_send(sender, reinterpret_cast<const char *>(&dummy), kMsgSize, 0);
        mq_close(sender);
    }
    if (logger_) logger_->info("ProcessMessageReceiver stop requested");
}

void ProcessMessageReceiver::operator()() {
    ProcessMessage msg{};
    while (running_) {
        ssize_t n = mq_receive(mq_, reinterpret_cast<char *>(&msg), kMsgSize, nullptr);
        if (n >= 0) {
            if (!running_) break;
            if (queue_) queue_->push(ThreadMessage{msg.type_, msg.payload_});
            if (logger_) logger_->info("ProcessMessageReceiver got message");
        } else if (errno == EINTR) {
            continue;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (logger_) logger_->error("mq_receive error");
        }
    }
    if (logger_) logger_->info("ProcessMessageReceiver loop end");
}

} // namespace device_reminder
