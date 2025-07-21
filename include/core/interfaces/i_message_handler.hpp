#include "infra/thread_message_operation/thread_message.hpp"

class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;
    virtual void handle(const device_reminder::ThreadMessage& msg) = 0;
};
