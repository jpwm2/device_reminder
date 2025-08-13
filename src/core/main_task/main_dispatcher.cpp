#include "main_task/main_dispatcher.hpp"

namespace device_reminder {

namespace {
std::string state_to_string(State state) {
    switch (state) {
    case State::WaitingHumanDetection:
        return "WaitingHumanDetection";
    case State::WaitingDevicePresenceResponse:
        return "WaitingDevicePresenceResponse";
    case State::WaitingDevicePresenceRetryResponse:
        return "WaitingDevicePresenceRetryResponse";
    case State::BluetoothScanRequestCooldown:
        return "BluetoothScanRequestCooldown";
    }
    return "Unknown";
}
} // namespace

MainDispatcher::MainDispatcher(std::shared_ptr<ILogger> logger,
                               std::shared_ptr<IMainHandler> handler,
                               IMessageType initial_state)
    : logger_(std::move(logger))
    , handler_(std::move(handler))
    , state_(initial_state) {}

void MainDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    if (logger_) logger_->info("[MainDispatcher::dispatch] start");
    try {
        if (!msg) {
            if (logger_) logger_->error("[MainDispatcher::dispatch] failed: null message");
            return;
        }

        auto event = msg->type();

        switch (event) {
        case MessageType::HumanDetected:
            if (state_ == State::WaitingHumanDetection) {
                if (handler_) handler_->start_device_detect();
                state_ = State::WaitingDevicePresenceResponse;
            }
            break;
        case MessageType::RespondDeviceFound:
            if (state_ == State::WaitingDevicePresenceResponse) {
                if (handler_) handler_->end_device_first_detect();
                state_ = State::WaitingHumanDetection;
            } else if (state_ == State::WaitingDevicePresenceRetryResponse) {
                if (handler_) handler_->end_device_retry_detect();
                state_ = State::WaitingHumanDetection;
            }
            break;
        case MessageType::RespondDeviceNotFound:
            if (state_ == State::WaitingDevicePresenceResponse) {
                if (handler_) handler_->retry_device_detect();
                state_ = State::BluetoothScanRequestCooldown;
            } else if (state_ == State::WaitingDevicePresenceRetryResponse) {
                if (handler_) handler_->send_bt_request();
                state_ = State::BluetoothScanRequestCooldown;
            }
            break;
        case MessageType::CooldownTimeout:
            if (state_ == State::BluetoothScanRequestCooldown) {
                if (handler_) handler_->send_bt_request();
                state_ = State::WaitingDevicePresenceRetryResponse;
            }
            break;
        case MessageType::ProcessingTimeout:
            if (state_ == State::WaitingDevicePresenceResponse) {
                if (handler_) handler_->end_device_first_detect();
                state_ = State::WaitingHumanDetection;
            } else if (state_ == State::BluetoothScanRequestCooldown) {
                if (handler_) handler_->end_device_retry_detect();
                state_ = State::WaitingHumanDetection;
            } else if (state_ == State::WaitingDevicePresenceRetryResponse) {
                if (handler_) handler_->end_device_retry_detect();
                state_ = State::WaitingHumanDetection;
            }
            break;
        default:
            break;
        }

        if (logger_) {
            logger_->info(std::string("[MainDispatcher::dispatch] success: state=") +
                          state_to_string(state_));
        }
    } catch (const std::exception& e) {
        if (logger_) logger_->error(std::string("[MainDispatcher::dispatch] failed: ") + e.what());
        throw;
    }
}

} // namespace device_reminder

