#pragma once
#include "infra/thread_message_operation/i_message_queue.hpp"
#include "infra/thread_message_operation/thread_message.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_message_operation/i_thread_dispatcher.hpp"
#include "infra/thread_message_operation/i_thread_receiver.hpp"
#include <memory>
#include <atomic>

namespace device_reminder {

class ThreadReceiver : public IThreadReceiver {
public:
    ThreadReceiver(std::shared_ptr<IThreadMessageQueue> queue,
                   std::shared_ptr<IThreadDispatcher> dispatcher,
                   std::shared_ptr<ILogger> logger = nullptr);
    void run() override;
    void stop() override;

private:
    std::shared_ptr<IThreadMessageQueue> queue_;
    std::shared_ptr<IThreadDispatcher> dispatcher_;
    std::atomic<bool> running_{true};
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
