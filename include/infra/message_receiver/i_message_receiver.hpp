#pragma once

#include <memory>
#include "message/i_message.hpp"

namespace device_reminder {

// メッセージ受信インターフェース
class IMessageReceiver {
public:
    virtual ~IMessageReceiver() = default;

    virtual std::unique_ptr<IMessage> receive() = 0;
};

} // namespace device_reminder
