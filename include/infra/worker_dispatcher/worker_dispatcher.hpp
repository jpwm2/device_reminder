#pragma once
#include "worker_dispatcher/i_worker_dispatcher.hpp"

#include <atomic>
#include <thread>

namespace device_reminder {

/**
 * @brief SysV メッセージキューを監視し、ハンドラへ通知するディスパッチャ
 */
class WorkerDispatcher final : public IWorkerDispatcher {
public:
    /**
     * @param mq_key         msgget で使用するキー
     * @param handler        受信後に呼ぶユーザハンドラ
     * @param accepted_type  受信対象とする mtype（デフォルト 1）
     *
     * 例: WorkerDispatcher disp(ftok("/tmp", 'Q'), cb);
     */
    WorkerDispatcher(key_t mq_key,
                     MessageHandler handler,
                     long accepted_type = 1);
    ~WorkerDispatcher() override;

    void start() override;
    void stop()  override;
    void join()  override;
    bool running() const noexcept override { return running_; }

private:
    void loop();            // 受信ループ

    int mq_id_ = -1;        // SysV MQ ID
    long accepted_type_;    // msgrcv フィルタ
    MessageHandler handler_;

    std::thread thread_;
    std::atomic<bool> running_{false};
};

} // namespace device_reminder
