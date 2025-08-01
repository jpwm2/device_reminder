#include "human_task/human_process.hpp"
#include "infra/logger/i_logger.hpp"

namespace device_reminder {

HumanProcess::HumanProcess(std::shared_ptr<IProcessQueue>    queue,
                           std::shared_ptr<IProcessReceiver> receiver,
                           std::shared_ptr<IProcessDispatcher> dispatcher,
                           std::shared_ptr<IProcessSender>   sender,
                           std::shared_ptr<IFileLoader>      file_loader,
                           std::shared_ptr<ILogger>          logger,
                           std::shared_ptr<IWatchDog>        watch_dog,
                           std::shared_ptr<IHandler>         handler,
                           std::shared_ptr<IHumanTask>       task)
    : ProcessBase(std::move(queue),
                  std::move(receiver),
                  std::move(dispatcher),
                  std::move(sender),
                  std::move(file_loader),
                  std::move(logger),
                  "HumanProcess")
    , human_task_(std::move(task))
    , handler_(std::move(handler))
    , watch_dog_(std::move(watch_dog))
{
    if (logger_) logger_->info("HumanProcess created");
}

int HumanProcess::run() {
    if (watch_dog_) watch_dog_->start();
    auto result = ProcessBase::run();
    if (watch_dog_) watch_dog_->stop();
    return result;
}

void HumanProcess::stop() {
    if (watch_dog_) watch_dog_->stop();
    ProcessBase::stop();
}

} // namespace device_reminder
