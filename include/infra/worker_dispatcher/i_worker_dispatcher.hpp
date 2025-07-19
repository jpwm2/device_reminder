#pragma once
#include <cstdint>
#include <functional>

namespace device_reminder {

/**
 * @brief 抽象ディスパッチャ
 *  - start()  : 受信スレッド開始
 *  - stop()   : 受信ループ終了要求（非同期）
 *  - join()   : スレッド合流
 *  - running(): 稼働状態確認
 */
class IWorkerDispatcher {
public:
    using MessageHandler = std::function<void(uint32_t /* msg */)>;

    virtual ~IWorkerDispatcher() = default;

    virtual void start()              = 0;
    virtual void stop()               = 0;
    virtual void join()               = 0;
    virtual bool running() const noexcept = 0;
};

} // namespace device_reminder
