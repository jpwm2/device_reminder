#pragma once
#include "infra/thread_operation/thread_receiver/i_thread_receiver.hpp"
#include "infra/thread_operation/thread_queue/i_thread_queue.hpp"
#include "infra/thread_operation/thread_dispatcher/i_thread_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include <atomic>
#include <memory>
#include <thread>

namespace device_reminder {

class ThreadReceiver : public IThreadReceiver {
public:
    ThreadReceiver(std::shared_ptr<ILogger> logger,
                   std::shared_ptr<IThreadQueue> queue,
                   std::shared_ptr<IThreadDispatcher> dispatcher);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IThreadQueue> queue_;
    std::shared_ptr<IThreadDispatcher> dispatcher_;
    std::atomic<bool> running_{false};
    std::thread worker_;
};

} // namespace device_reminder
