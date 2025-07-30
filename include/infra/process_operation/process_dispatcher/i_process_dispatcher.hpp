#pragma once

#include <memory>

namespace device_reminder {

class IProcessMessage;

/**
 * @brief プロセスメッセージディスパッチャのインタフェース
 */
class IProcessDispatcher {
public:
    virtual ~IProcessDispatcher() = default;

    virtual void dispatch(std::shared_ptr<IProcessMessage> msg) = 0;
};

} // namespace device_reminder
