#pragma once

#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/message/message_type.hpp"
#include "infra/message/i_message.hpp"

#include <functional>
#include <memory>
#include <unordered_map>

namespace device_reminder {

/**
 * @brief メッセージを種別に応じてディスパッチするクラス
 */
class ProcessDispatcher final : public IProcessDispatcher {
public:
    using HandlerMap =
        std::unordered_map<MessageType,
                           std::function<void(std::shared_ptr<IMessage>)>>;

    ProcessDispatcher(std::shared_ptr<ILogger> logger, HandlerMap handler_map);

    void dispatch(std::shared_ptr<IMessage> msg) override;

private:
    std::shared_ptr<ILogger> logger_;
    HandlerMap               handler_map_;
};

} // namespace device_reminder
