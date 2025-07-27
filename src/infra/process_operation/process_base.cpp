#include <chrono>
#include <csignal>
#include <thread>

#include "process/process_base.hpp"
#include "process_message_operation/i_process_message_receiver.hpp"
#include "process_message_operation/i_process_message_sender.hpp"
#include "worker_dispatcher/i_worker_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include <functional>

std::atomic<bool> ProcessBase::g_stop_flag{false};

ProcessBase::ProcessBase(std::shared_ptr<IProcessMessageReceiver> receiver,
                         std::shared_ptr<IWorkerDispatcher> dispatcher,
                         std::shared_ptr<IProcessMessageSender> sender,
                         std::function<int()> load_setting_value,
                         std::shared_ptr<ILogger> logger)
    : receiver_(std::move(receiver))
    , dispatcher_(std::move(dispatcher))
    , sender_(std::move(sender))
    , load_setting_value_(std::move(load_setting_value))
    , logger_(std::move(logger))

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
    if (dispatcher_) dispatcher_->start();

    if (logger_) logger_->info("ProcessBase run start");

    while (!g_stop_flag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    receiver_->stop();
    if (dispatcher_) dispatcher_->stop();
    if (sender_) sender_->stop();
    recv_thr.join();
    if (dispatcher_) dispatcher_->join();
    if (logger_) logger_->info("ProcessBase run end");
    return 0;
}

void ProcessBase::stop()
{
    g_stop_flag.store(true);
    if (logger_) logger_->info("ProcessBase stop requested");
}

int ProcessBase::priority() const noexcept
{
    if (load_setting_value_) return load_setting_value_();
    return 0;
}
