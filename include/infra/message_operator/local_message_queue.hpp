#pragma once
#include "i_message_queue.hpp"
#include "infra/logger/i_logger.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <memory>

namespace device_reminder {

class LocalMessageQueue : public IMessageQueue {
public:
    explicit LocalMessageQueue(std::shared_ptr<ILogger> logger = nullptr);
    ~LocalMessageQueue() override = default;

    bool push(const Message& msg) override;
    std::optional<Message> pop() override;
    bool pop(Message& out) override;

    bool is_open() const noexcept override;
    void close() override;

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<Message> q_;
    bool open_{true};
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
