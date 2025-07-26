#pragma once
#include "infra/process_operation/worker_dispatcher/i_worker_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include <memory>

#include <atomic>
#include <thread>
#include "thread_message_operation/i_message_queue.hpp"

namespace device_reminder {

/**
 * @brief 指定されたメッセージキューを監視し、ハンドラへ通知するディスパッチャ
 */
class WorkerDispatcher final : public IWorkerDispatcher {
public:
    /**
     * @param queue   監視対象となるメッセージキュー
     * @param handler メッセージ取得後に呼ばれるハンドラ
     */
    WorkerDispatcher(std::shared_ptr<IThreadMessageQueue> queue,
                     MessageHandler handler,
                     std::shared_ptr<ILogger> logger = nullptr);
    ~WorkerDispatcher() override;

    void start() override;
    void stop()  override;
    void join()  override;
    bool running() const noexcept override { return running_; }

    enum class State { IDLE, RUNNING };
    State state() const noexcept { return state_; }
private:
    void loop();            // 受信ループ

    std::shared_ptr<IThreadMessageQueue> queue_;
    MessageHandler handler_;
    std::shared_ptr<ILogger> logger_;
    State                   state_{State::IDLE};

    std::thread thread_;
    std::atomic<bool> running_{false};
};

} // namespace device_reminder
