#pragma once
#include "infra/thread_operation/thread_receiver/i_thread_receiver.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include "infra/thread_operation/thread_message/i_thread_message.hpp"
#include "infra/thread_operation/thread_dispatcher/i_thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>
#include <atomic>

namespace device_reminder {

class ThreadReceiver : public IThreadReceiver {
public:
    ThreadReceiver(std::shared_ptr<IThreadQueue> queue,
                   std::shared_ptr<IThreadDispatcher> dispatcher,
                   std::shared_ptr<ILogger> logger = nullptr);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IThreadQueue> queue_;
    std::shared_ptr<IThreadDispatcher> dispatcher_;
    std::atomic<bool> running_{true};
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
