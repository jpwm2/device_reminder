#pragma once

#include <string>
#include "message/i_message.hpp"

namespace device_reminder {

// メッセージ送信インターフェース
class IMessageSender {
public:
    virtual ~IMessageSender() = default;

    // 宛先タスク名とメッセージを指定して送信
    virtual bool send(const std::string& destination_task, const IMessage& message) = 0;
};

} // namespace device_reminder
