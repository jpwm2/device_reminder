#pragma once

#include "process/i_process_base.hpp"
#include "infra/logger/i_logger.hpp"
#include "process_message_operation/i_process_message_receiver.hpp"
#include "process_message_operation/i_process_message_sender.hpp"
#include "worker_dispatcher/i_worker_dispatcher.hpp"

#include <atomic>
#include <functional>
#include <memory>

class ProcessBase : public IProcessBase {
public:
    ProcessBase(std::shared_ptr<IProcessMessageReceiver> receiver,
                std::shared_ptr<IWorkerDispatcher> dispatcher,
                std::shared_ptr<IProcessMessageSender> sender,
                std::function<int()> load_setting_value,
                std::shared_ptr<ILogger> logger);

    int  run()  override;  ///< メインループ
    void stop() override;  ///< 外部停止
    int  priority() const noexcept override;

private:
    static std::atomic<bool> g_stop_flag;           ///< 全スレッド共通の終了フラグ

    std::shared_ptr<IProcessMessageReceiver> receiver_;
    std::shared_ptr<IWorkerDispatcher>       dispatcher_;
    std::shared_ptr<IProcessMessageSender>   sender_;
    std::function<int()>                     load_setting_value_;
    std::shared_ptr<ILogger>                 logger_;
};
