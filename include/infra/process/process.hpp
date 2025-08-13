#pragma once

#include "infra/message/message_receiver.hpp"
#include "infra/file_loader.hpp"
#include "infra/logger.hpp"

#include <atomic>
#include <memory>

namespace device_reminder {

class IProcess {
public:
    virtual ~IProcess() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};

class Process : public IProcess {
public:
    Process(std::shared_ptr<IMessageReceiver> receiver,
            std::shared_ptr<IFileLoader> file_loader,
            std::shared_ptr<ILogger> logger);

    void run() override;
    void stop() override;

private:
    std::shared_ptr<IMessageReceiver> receiver_;
    std::shared_ptr<IFileLoader> file_loader_;
    std::shared_ptr<ILogger> logger_;
    std::atomic<bool> running_{false};
};

} // namespace device_reminder

