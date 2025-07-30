#pragma once

#include "infra/process_operation/process_dispatcher/i_process_dispatcher.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/process_message/process_message_type.hpp"

#include <functional>
#include <memory>
#include <unordered_map>

namespace device_reminder {

/**
 * @brief プロセスメッセージを種別に応じてディスパッチするクラス
 */
class ProcessDispatcher final : public IProcessDispatcher {
public:
    using HandlerMap =
        std::unordered_map<ProcessMessageType,
                           std::function<void(std::shared_ptr<IProcessMessage>)>>;

    ProcessDispatcher(std::shared_ptr<ILogger> logger, HandlerMap handler_map);

    void dispatch(std::shared_ptr<IProcessMessage> msg) override;

private:
    std::shared_ptr<ILogger> logger_;
    HandlerMap handler_map_;
};

} // namespace device_reminder
