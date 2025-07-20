#pragma once

#include "process/i_process_base.hpp"
#include "infra/logger/i_logger.hpp"

#include <atomic>
#include <memory>
#include <string>

#include "message_operator/message_receiver.hpp"
#include "worker_dispatcher/worker_dispatcher.hpp"

class ProcessBase : public IProcessBase {
public:
    ProcessBase(const std::string& mq_name,
                std::shared_ptr<IMessageHandler> handler,
                std::shared_ptr<ILogger> logger,
                int priority = 0);

    int  run()  override;  ///< メインループ
    void stop() override;  ///< 外部停止
    int  priority() const noexcept override { return priority_; }

private:
    static std::atomic<bool> g_stop_flag;           ///< 全スレッド共通の終了フラグ

    std::shared_ptr<IMessageQueue>         queue_;
    std::unique_ptr<MessageReceiver>       receiver_;
    std::unique_ptr<WorkerDispatcher>      worker_;
    std::shared_ptr<ILogger>               logger_;
    int                                    priority_{0};
};
