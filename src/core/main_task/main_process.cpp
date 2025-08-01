#include "main_task/main_process.hpp"

namespace device_reminder {

MainProcess::MainProcess(std::shared_ptr<IProcessReceiver> receiver,
                         std::shared_ptr<IProcessSender> sender,
                         std::shared_ptr<IProcessQueue> queue,
                         std::shared_ptr<IFileLoader> file_loader,
                         std::shared_ptr<ILogger> logger,
                         std::shared_ptr<IWatchDog> watchdog)
    : ProcessBase(std::move(queue),
                  std::move(receiver),
                  nullptr,
                  std::move(sender),
                  std::move(file_loader),
                  std::move(logger),
                  "MainProcess")
    , watchdog_(std::move(watchdog))
{
    if (logger_) logger_->info("MainProcess created");
}

int MainProcess::run()
{
    if (watchdog_) watchdog_->start();
    auto result = ProcessBase::run();
    if (watchdog_) watchdog_->stop();
    return result;
}

void MainProcess::stop()
{
    if (watchdog_) watchdog_->stop();
    ProcessBase::stop();
}

} // namespace device_reminder
