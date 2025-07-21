#pragma once
#include "infra/process_message_operation/process_message.hpp"
#include "infra/process_message_operation/process_message_queue.hpp" // forward decl
#include "infra/thread_message_operation/i_message_queue.hpp"
#include "infra/logger/i_logger.hpp"
#include <mqueue.h>
#include <atomic>
#include <memory>
#include <string>

namespace device_reminder {

class ProcessMessageReceiver {
public:
    ProcessMessageReceiver(const std::string& mq_name,
                           std::shared_ptr<IThreadMessageQueue> queue,
                           std::shared_ptr<ILogger> logger = nullptr);
    ~ProcessMessageReceiver();

    void operator()();
    void stop();
    bool running() const noexcept { return running_; }

private:
    static constexpr long kMsgSize = PROCESS_MESSAGE_SIZE;
    static constexpr long kMaxMsgs = 10;

    mqd_t open_queue(const std::string& name);
    void  close_queue();

    mqd_t                              mq_{};
    std::string                        mq_name_;
    std::shared_ptr<IThreadMessageQueue> queue_;
    std::atomic<bool>                  running_{true};
    std::shared_ptr<ILogger>           logger_;
};

} // namespace device_reminder
