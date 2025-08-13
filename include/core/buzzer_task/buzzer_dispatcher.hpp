#pragma once

#include "buzzer_task/buzzer_handler.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/message/message.hpp"

#include <memory>

namespace device_reminder {

class IBuzzerDispatcher {
public:
  IBuzzerDispatcher(std::shared_ptr<ILogger> logger,
                    std::shared_ptr<IBuzzerHandler> handler,
                    MessageType message_type);
  virtual ~IBuzzerDispatcher() = default;

  virtual void dispatch(std::shared_ptr<IMessage> msg) = 0;

protected:
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IBuzzerHandler> handler_;
  MessageType message_type_;
};

class BuzzerDispatcher : public IBuzzerDispatcher {
public:
  BuzzerDispatcher(std::shared_ptr<ILogger> logger,
                   std::shared_ptr<IBuzzerHandler> handler,
                   MessageType message_type);

  void dispatch(std::shared_ptr<IMessage> msg) override;
};

} // namespace device_reminder
