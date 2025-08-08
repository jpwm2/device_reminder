#include "infra/process_operation/process_base/process_base.hpp"

#include <sys/resource.h>

#include <string>

namespace device_reminder {

ProcessBase::ProcessBase(std::shared_ptr<IProcessReceiver> receiver,
                         std::shared_ptr<IProcessQueue> queue,
                         std::shared_ptr<IFileLoader> priority_loader,
                         std::shared_ptr<ILogger> logger,
                         std::shared_ptr<IWatchDog> watchdog)
    : receiver_(std::move(receiver))
    , queue_(std::move(queue))
    , priority_loader_(std::move(priority_loader))
    , logger_(std::move(logger))
    , watchdog_(std::move(watchdog))
{
}

void ProcessBase::run()
{
    try {
        if (logger_) logger_->info("ProcessBase run start");

        if (priority_loader_) {
            int prio = priority_loader_->load_int("priority");
            setpriority(PRIO_PROCESS, 0, prio);
        }

        if (watchdog_) watchdog_->start();
        if (receiver_) receiver_->run();

        if (logger_) logger_->info("ProcessBase run success");
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string{"ProcessBase run error: "} + e.what());
        throw;
    }
}

void ProcessBase::stop()
{
    if (logger_) logger_->info("ProcessBase stop start");

    if (receiver_) receiver_->stop();
    if (watchdog_) watchdog_->stop();

    if (logger_) logger_->info("ProcessBase stop complete");
}

} // namespace device_reminder
