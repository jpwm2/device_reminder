#include "infra/message/message.hpp"

class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;
    virtual void handle(const device_reminder::Message& msg) = 0;
};
