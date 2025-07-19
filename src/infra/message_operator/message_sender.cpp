#include "message_operator/message_sender.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <system_error>
#include <sys/stat.h>
#include <unistd.h>

namespace device_reminder {
namespace {

//--------------------------------------------------------------------
// POSIX メッセージキューを「送信専用」で開く（無ければ作成）
//--------------------------------------------------------------------
mqd_t open_or_create_queue(const std::string& name, long max_messages)
{
    mq_attr attr{};
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = max_messages;
    attr.mq_msgsize = static_cast<long>(MESSAGE_SIZE);
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(name.c_str(), O_WRONLY | O_CREAT, 0644, &attr);
    if (mq == static_cast<mqd_t>(-1)) {
        throw std::system_error(errno, std::system_category(),
                                "mq_open failed for " + name);
    }
    return mq;
}

} // anonymous namespace
//--------------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------------
MessageSender::MessageSender(const std::string& queue_name,
                             long               max_messages,
                             std::shared_ptr<TSQueue<Message>> q)
    : queue_name_{queue_name},
      queue_{std::move(q)},
      mq_{open_or_create_queue(queue_name_, max_messages)}
{
    thread_ = std::thread(&MessageSender::run, this);
}
//--------------------------------------------------------------------
MessageSender::~MessageSender()
{
    stop();
}
//--------------------------------------------------------------------
bool MessageSender::enqueue(const Message& msg)
{
    if (!running_) return false;
    return queue_->push(msg);
}
//--------------------------------------------------------------------
void MessageSender::stop()
{
    if (!running_.exchange(false)) return;  // すでに停止処理済み
    queue_->shutdown();                     // pop_wait を解除
    if (thread_.joinable()) thread_.join();
    if (mq_ != static_cast<mqd_t>(-1)) {
        mq_close(mq_);
        mq_ = static_cast<mqd_t>(-1);
    }
}
//--------------------------------------------------------------------
void MessageSender::run()
{
    Message msg{};
    while (running_) {
        if (!queue_->pop_wait(msg)) {
            // shutdown() でキューが閉じられた
            break;
        }
        if (mq_send(mq_,
                    reinterpret_cast<const char*>(&msg),
                    MESSAGE_SIZE,
                    /*prio=*/0) == -1)
        {
            // ポリシーに応じてリトライ／ドロップを選択可
            throw std::system_error(errno, std::system_category(),
                                    "mq_send failed");
        }
    }
}

} // namespace device_reminder
