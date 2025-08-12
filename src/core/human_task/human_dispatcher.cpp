#include "human_task/human_dispatcher.hpp"

#include <stdexcept>
#include <string>

namespace device_reminder {

IHumanDispatcher::IHumanDispatcher(std::shared_ptr<ILogger> logger,
                                   std::shared_ptr<IHumanHandler> handler,
                                   MessageType message_type)
    : logger_(std::move(logger)),
      handler_(std::move(handler)),
      message_type_(message_type) {}

HumanDispatcher::HumanDispatcher(std::shared_ptr<ILogger> logger,
                                 std::shared_ptr<IHumanHandler> handler,
                                 MessageType message_type)
    : IHumanDispatcher(std::move(logger), std::move(handler), message_type) {}

const char* HumanDispatcher::state_to_string(State state) const {
    switch (state) {
    case State::Detecting:
        return "人検知中";
    case State::Stopped:
        return "人検知停止中";
    }
    return "";
}

void HumanDispatcher::dispatch(std::shared_ptr<IMessage> msg) {
    try {
        if (!msg) {
            if (logger_) logger_->error("dispatch start: msg is null");
            throw std::invalid_argument("msg is null");
        }

        State       prev_state = state_;
        MessageType type       = msg->type();
        if (logger_) {
            logger_->info(std::string("dispatch start: state=") +
                          state_to_string(state_) +
                          ", msg.type=" +
                          std::to_string(static_cast<int>(type)));
        }

        std::string action;

        switch (type) {
        case MessageType::HumanDetected:
            if (handler_) handler_->get_detect();
            action = "get_detect";
            if (state_ == State::Detecting) {
                state_ = State::Stopped;
            } else if (logger_) {
                logger_->info("state mismatch, transition skipped");
            }
            break;
        case MessageType::CooldownTimeout:
            if (handler_) handler_->start_detect();
            action = "start_detect";
            if (state_ == State::Stopped) {
                state_ = State::Detecting;
            } else if (logger_) {
                logger_->info("state mismatch, transition skipped");
            }
            break;
        case MessageType::StopHumanDetection:
            if (state_ == State::Detecting) {
                state_ = State::Stopped;
            } else if (logger_) {
                logger_->info("state mismatch, transition skipped");
            }
            action = "none";
            break;
        case MessageType::StartHumanDetection:
            if (handler_) handler_->start_detect();
            action = "start_detect";
            if (state_ == State::Stopped) {
                state_ = State::Detecting;
            } else if (logger_) {
                logger_->info("state mismatch, transition skipped");
            }
            break;
        default:
            if (logger_) logger_->warn("undefined message type");
            action = "none";
            break;
        }

        if (logger_) {
            logger_->info(std::string("dispatch success: ") +
                          state_to_string(prev_state) + " -> " +
                          state_to_string(state_) +
                          ", action=" + action);
        }
    } catch (...) {
        if (logger_) logger_->error("dispatch failed");
        throw;
    }
}

} // namespace device_reminder

