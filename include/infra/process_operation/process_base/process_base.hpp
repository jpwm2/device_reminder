#pragma once

#include "infra/process_operation/process_base/i_process_base.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/file_loader/i_file_loader.hpp"

#include <atomic>
#include <memory>
#include <string>

namespace device_reminder {

class IProcessDispatcher;

class ProcessBase : public IProcessBase {
public:
    ProcessBase(std::shared_ptr<IProcessQueue>    queue,
                std::shared_ptr<IProcessReceiver> receiver,
                std::shared_ptr<IProcessDispatcher> dispatcher,
                std::shared_ptr<IProcessSender>   sender,
                std::shared_ptr<IFileLoader>             file_loader,
                std::shared_ptr<ILogger>                 logger,
                std::string                              process_name);

    int  run() override;  ///< メインループ
    void stop() override;  ///< 外部停止
    int  priority() const noexcept override;

protected:
    static std::atomic<bool> g_stop_flag;           ///< 全スレッド共通の終了フラグ

    std::shared_ptr<IProcessQueue>    queue_;
    std::shared_ptr<IProcessReceiver> receiver_;
    std::shared_ptr<IProcessDispatcher> dispatcher_;
    std::shared_ptr<IProcessSender>   sender_;
    std::shared_ptr<IFileLoader>             file_loader_;
    std::shared_ptr<ILogger>                 logger_;
    std::string                              process_name_;
    int                                      priority_{0};
};

} // namespace device_reminder
