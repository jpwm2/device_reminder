#pragma once

#include "infra/message/i_message.hpp"
#include <memory>

namespace device_reminder {

/**
 * @brief プロセスメッセージディスパッチャのインタフェース
 */
class IProcessDispatcher {
public:
    virtual ~IProcessDispatcher() = default;

    /**
     * @brief 受信したメッセージをディスパッチする
     * @param msg ディスパッチ対象のメッセージ
     */
    virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;
};

} // namespace device_reminder
