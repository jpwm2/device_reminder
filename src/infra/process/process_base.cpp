#include <chrono>
#include <csignal>

#include "process/process_base.hpp"
#include "message_operator/i_message_queue.hpp"
#include "interfaces/i_message_handler.hpp"

std::atomic<bool> ProcessBase::g_stop_flag{false};

ProcessBase::ProcessBase(std::shared_ptr<IMessageQueue> /*mq*/,
                         std::shared_ptr<IMessageHandler> /*handler*/)
{
    // Ctrl‑C (SIGINT) を捕捉して終了フラグを立てる
    struct sigaction sa {};
    sa.sa_handler = [](int){ g_stop_flag.store(true); };
    sigaction(SIGINT, &sa, nullptr);
}

int ProcessBase::run()
{
    while (!g_stop_flag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return 0;
}

void ProcessBase::stop()
{
    g_stop_flag.store(true);
}
