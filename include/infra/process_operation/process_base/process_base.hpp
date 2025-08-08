#pragma once

#include "infra/process_operation/process_base/i_process_base.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/watch_dog/i_watch_dog.hpp"

#include <memory>

namespace device_reminder {

/**
 * @brief プロセス共通の基底クラス
 */
class ProcessBase : public IProcessBase {
public:
    ProcessBase(std::shared_ptr<IProcessReceiver> receiver,
                std::shared_ptr<IProcessQueue> queue,
                std::shared_ptr<IFileLoader> priority_loader,
                std::shared_ptr<ILogger> logger,
                std::shared_ptr<IWatchDog> watchdog);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IProcessReceiver> receiver_;
    std::shared_ptr<IProcessQueue> queue_;
    std::shared_ptr<IFileLoader> priority_loader_;
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IWatchDog> watchdog_;
};

} // namespace device_reminder
