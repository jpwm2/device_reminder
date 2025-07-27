#pragma once
#include "infra/thread_operation/i_thread_sender.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

namespace device_reminder {

class ThreadSender : public IThreadSender {
public:
    ThreadSender(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IThreadQueue<std::shared_ptr<IThreadMessage>>> queue,
                 std::shared_ptr<IThreadMessage> message);
    void send() override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IThreadQueue<std::shared_ptr<IThreadMessage>>> queue_;
    std::shared_ptr<IThreadMessage> message_;
};

} // namespace device_reminder
