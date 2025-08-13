#include "buzzer_task/buzzer_dispatcher.hpp"

#include <stdexcept>
#include <string>

namespace device_reminder {

IBuzzerDispatcher::IBuzzerDispatcher(std::shared_ptr<ILogger> logger,
                                     std::shared_ptr<IBuzzerHandler> handler,
                                     MessageType message_type)
    : logger_(std::move(logger)), handler_(std::move(handler)),
      message_type_(message_type) {}

BuzzerDispatcher::BuzzerDispatcher(std::shared_ptr<ILogger> logger,
                                   std::shared_ptr<IBuzzerHandler> handler,
                                   MessageType message_type)
    : IBuzzerDispatcher(std::move(logger), std::move(handler), message_type) {}

void BuzzerDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
  if (logger_)
    logger_->info("[BuzzerDispatcher::dispatch] start");
  try {
    if (!msg) {
      if (logger_)
        logger_->error("[BuzzerDispatcher::dispatch] failed: null message");
      throw std::invalid_argument("msg is null");
    }

    auto type = msg->type();

    switch (type) {
    case MessageType::StartBuzzing:
      if (handler_)
        handler_->start_buzzing_and_start_timer();
      if (logger_)
        logger_->info("[BuzzerDispatcher::dispatch] success: StartBuzzing");
      break;
    case MessageType::StopBuzzing:
    case MessageType::BuzzTimeout:
      if (handler_)
        handler_->stop_buzzing_and_stop_timer();
      if (logger_)
        logger_->info(
            type == MessageType::StopBuzzing
                ? "[BuzzerDispatcher::dispatch] success: StopBuzzing"
                : "[BuzzerDispatcher::dispatch] success: BuzzTimeout");
      break;
    default:
      if (logger_)
        logger_->warn("[BuzzerDispatcher::dispatch] unsupported message type");
      break;
    }
  } catch (const std::exception &e) {
    if (logger_)
      logger_->error(std::string("[BuzzerDispatcher::dispatch] failed: ") +
                     e.what());
    throw;
  }
}

} // namespace device_reminder
