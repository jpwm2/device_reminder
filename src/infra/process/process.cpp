#include <chrono>
#include <csignal>
#include <thread>

#include "infra/process_operation/process_base/process_base.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

std::atomic<bool> ProcessBase::g_stop_flag{false};

ProcessBase::ProcessBase(std::shared_ptr<IProcessQueue>    queue,
                         std::shared_ptr<IProcessReceiver> receiver,
                         std::shared_ptr<IProcessDispatcher> dispatcher,
                         std::shared_ptr<IProcessSender>   sender,
                         std::shared_ptr<IFileLoader>             file_loader,
                         std::shared_ptr<ILogger>                 logger,
                         std::string                              process_name)
    : queue_(std::move(queue))
    , receiver_(std::move(receiver))
    , dispatcher_(std::move(dispatcher))
    , sender_(std::move(sender))
    , file_loader_(std::move(file_loader))
    , logger_(std::move(logger))
    , process_name_(std::move(process_name))
    , priority_{file_loader_ ? file_loader_->load_int("priority") : 0}

{
    // Ctrl‑C (SIGINT) を捕捉して終了フラグを立てる
    struct sigaction sa {};
    sa.sa_handler = [](int){ g_stop_flag.store(true); };
    sigaction(SIGINT, &sa, nullptr);
    if (logger_) logger_->info("ProcessBase initialized");
}

int ProcessBase::run()
{
    running_.store(true);
    if (receiver_) receiver_->run();

    if (logger_) logger_->info("ProcessBase run start");

    while (!g_stop_flag.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (receiver_) receiver_->stop();
    if (logger_) logger_->info("ProcessBase run end");
    running_.store(false);
    return 0;
}

void ProcessBase::stop()
{
    g_stop_flag.store(true);
    if (logger_) logger_->info("ProcessBase stop requested");
}

int ProcessBase::priority() const noexcept
{
    return priority_;
}

} // namespace device_reminder
