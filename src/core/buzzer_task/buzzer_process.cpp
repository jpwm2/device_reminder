#include "buzzer_task/buzzer_process.hpp"

namespace device_reminder {

BuzzerProcess::BuzzerProcess(std::shared_ptr<IProcessReceiver> receiver,
                             std::shared_ptr<IProcessSender> sender,
                             std::shared_ptr<IProcessQueue> queue,
                             std::shared_ptr<IFileLoader> file_loader,
                             std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IWatchDog> watch_dog)
    : ProcessBase(std::move(queue),
                  std::move(receiver),
                  nullptr,
                  std::move(sender),
                  std::move(file_loader),
                  std::move(logger),
                  "BuzzerProcess"),
      watch_dog_(std::move(watch_dog))
{
    if (logger_) logger_->info("BuzzerProcess created");
}

int BuzzerProcess::run()
{
    if (watch_dog_) watch_dog_->start();
    auto result = ProcessBase::run();
    if (watch_dog_) watch_dog_->stop();
    return result;
}

void BuzzerProcess::stop()
{
    if (watch_dog_) watch_dog_->stop();
    ProcessBase::stop();
}

} // namespace device_reminder
