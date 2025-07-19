#include <string>

class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;
    virtual void handle(const std::string& msg) = 0;
};
