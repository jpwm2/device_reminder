#pragma once

namespace device_reminder {

/**
 * @brief スレッドキューへメッセージを送信するインタフェース
 */
class IThreadSender {
public:
    virtual ~IThreadSender() = default;

    /**
     * @brief 保持されたメッセージを送信する
     */
    virtual void send() = 0;
};

} // namespace device_reminder
