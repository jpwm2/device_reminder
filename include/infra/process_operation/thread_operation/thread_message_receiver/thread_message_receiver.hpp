#pragma once
#include "infra/thread_message_operation/i_message_queue.hpp"
#include "infra/thread_message_operation/thread_message.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/thread_message_operation/i_thread_message_receiver.hpp"
#include <functional>
#include <memory>
#include <atomic>

namespace device_reminder {

class ThreadMessageReceiver : public IThreadMessageReceiver {
public:
    using MessageHandler = std::function<void(const ThreadMessage&)>;

    ThreadMessageReceiver(std::shared_ptr<IThreadMessageQueue> queue,
                          MessageHandler handler,
                          std::shared_ptr<ILogger> logger = nullptr);
    ~ThreadMessageReceiver();

    void operator()();
    void stop();
    bool running() const noexcept { return running_; }

private:
    std::shared_ptr<IThreadMessageQueue> queue_;
    MessageHandler                       handler_;
    std::atomic<bool>                    running_{true};
    std::shared_ptr<ILogger>             logger_;
};

} // namespace device_reminder
