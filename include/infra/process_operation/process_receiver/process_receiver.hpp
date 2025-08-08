#pragma once
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"

#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace device_reminder {

/**
 * @brief キューからメッセージを受信しディスパッチする受信モジュール
 */
class ProcessReceiver : public IProcessReceiver {
public:
    ProcessReceiver(std::shared_ptr<ILogger> logger,
                    std::shared_ptr<IProcessQueue> queue,
                    std::shared_ptr<IProcessDispatcher> dispatcher);
    ~ProcessReceiver();

    void run() override;  ///< 受信スレッドを開始
    void stop() override; ///< 受信スレッドを停止

private:
    void loop();

    std::shared_ptr<ILogger>           logger_;
    std::shared_ptr<IProcessQueue>     queue_;
    std::shared_ptr<IProcessDispatcher> dispatcher_;
    std::atomic<bool>                  running_{false};
    std::thread                        worker_;
};

} // namespace device_reminder
