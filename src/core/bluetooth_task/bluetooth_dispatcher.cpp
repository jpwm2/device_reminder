#include "core/bluetooth_task/bluetooth_dispatcher.hpp"
#include "core/bluetooth_task/bluetooth_handler.hpp"

#include <stdexcept>
#include <string>

namespace device_reminder {

IBluetoothDispatcher::IBluetoothDispatcher(std::shared_ptr<ILogger> logger,
                                           std::shared_ptr<IBluetoothHandler> handler,
                                           MessageType message_type)
    : logger_(std::move(logger)),
      handler_(std::move(handler)),
      message_type_(message_type) {}

BluetoothDispatcher::BluetoothDispatcher(std::shared_ptr<ILogger> logger,
                                         std::shared_ptr<IBluetoothHandler> handler,
                                         MessageType message_type)
    : IBluetoothDispatcher(std::move(logger), std::move(handler), message_type) {}

const char* BluetoothDispatcher::state_to_string(State state) const {
    switch (state) {
    case State::Ready:
        return "スキャン待機";
    case State::Cooldown:
        return "クールダウン中";
    }
    return "";
}

void BluetoothDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    try {
        if (!msg) {
            if (logger_) logger_->error("dispatch start: msg is null");
            throw std::invalid_argument("msg is null");
        }

        MessageType type = msg->type();
        if (logger_) {
            logger_->info(std::string("dispatch start: state=") +
                          state_to_string(state_) +
                          ", msg.type=" +
                          std::to_string(static_cast<int>(type)));
        }

        if (type != message_type_) {
            if (logger_) logger_->info("dispatch ignore: not scan request");
            return;
        }

        if (state_ != State::Ready) {
            if (logger_)
                logger_->info(std::string("dispatch ignore: state=") +
                               state_to_string(state_));
            return;
        }

        if (logger_) {
            logger_->info(std::string("dispatch processing: ") +
                          msg->to_string());
        }

        if (handler_) handler_->scan();

        state_ = State::Cooldown;

        if (logger_) logger_->info("dispatch success: scan executed");
    } catch (...) {
        if (logger_) logger_->error("dispatch failed");
        throw;
    }
}

} // namespace device_reminder

