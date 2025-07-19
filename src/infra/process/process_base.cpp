#include <chrono>
#include <csignal>
#include <thread>

#include "process/process_base.hpp"
#include "message_operator/i_message_queue.hpp"
#include "interfaces/i_message_handler.hpp"

std::atomic<bool> ProcessBase::g_stop_flag{false};

ProcessBase::ProcessBase(const std::string& mq_name,
                         std::shared_ptr<IMessageHandler> handler)
    : receiver_{std::make_unique<MessageReceiver>(mq_name, nullptr)},
      worker_  {std::make_unique<WorkerDispatcher>(0, handler)}
{
    // Ctrl‑C (SIGINT) を捕捉して終了フラグを立てる
    struct sigaction sa {};
    sa.sa_handler = [](int){ g_stop_flag.store(true); };
    sigaction(SIGINT, &sa, nullptr);
}

int ProcessBase::run()
{
    std::thread recv_thr{std::ref(*receiver_)};
    std::thread work_thr{std::ref(*worker_)};

    while (!g_stop_flag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    receiver_->stop();
    worker_->stop();
    recv_thr.join();
    work_thr.join();
    return 0;
}

void ProcessBase::stop()
{
    g_stop_flag.store(true);
}
