#pragma once

#include "process/i_process_base.hpp"

#include <atomic>
#include <memory>
#include <string>

#include "message_operator/message_receiver.hpp"
#include "worker_dispatcher/worker_dispatcher.hpp"

class ProcessBase : public IProcessBase {
public:
    ProcessBase(std::shared_ptr<IMessageQueue> mq,
                std::shared_ptr<IMessageHandler> handler);

    int  run()  override;  ///< メインループ
    void stop() override;  ///< 外部停止

private:
    static std::atomic<bool> g_stop_flag;           ///< 全スレッド共通の終了フラグ

    std::unique_ptr<MessageReceiver>       receiver_;
    std::unique_ptr<WorkerDispatcher>      worker_;
};
