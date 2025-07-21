#include <chrono>
#include <csignal>
#include <thread>

#include "process/process_base.hpp"
#include "thread_message_operation/i_message_queue.hpp"
#include "thread_message_operation/thread_message_queue.hpp"
#include "process_message_operation/process_message_receiver.hpp"
#include "interfaces/i_message_handler.hpp"
#include "infra/logger/i_logger.hpp"

std::atomic<bool> ProcessBase::g_stop_flag{false};

ProcessBase::ProcessBase(const std::string& mq_name,
                         std::shared_ptr<IMessageHandler> handler,
                         std::shared_ptr<ILogger> logger,
                         int priority)
    : queue_{std::make_shared<ThreadMessageQueue>(logger)},
      receiver_{std::make_unique<ProcessMessageReceiver>(mq_name, queue_, logger)},
      worker_{std::make_unique<WorkerDispatcher>(
          queue_,
          [handler](const ThreadMessage& msg) {
              if (!handler) return;
              handler->handle(msg);
          },
          logger)},
      logger_(std::move(logger)),
      priority_(priority)

{
    // Ctrl‑C (SIGINT) を捕捉して終了フラグを立てる
    struct sigaction sa {};
    sa.sa_handler = [](int){ g_stop_flag.store(true); };
    sigaction(SIGINT, &sa, nullptr);
    if (logger_) logger_->info("ProcessBase initialized");
}

int ProcessBase::run()
{
    std::thread recv_thr{std::ref(*receiver_)};
    std::thread work_thr{std::ref(*worker_)};

    if (logger_) logger_->info("ProcessBase run start");

    while (!g_stop_flag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    receiver_->stop();
    worker_->stop();
    recv_thr.join();
    work_thr.join();
    if (logger_) logger_->info("ProcessBase run end");
    return 0;
}

void ProcessBase::stop()
{
    g_stop_flag.store(true);
    if (logger_) logger_->info("ProcessBase stop requested");
}
