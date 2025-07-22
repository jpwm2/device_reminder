#pragma once
#include "infra/process_message_operation/process_message.hpp"
#include "infra/logger/i_logger.hpp"
#include "infra/process_message_operation/i_process_message_queue.hpp"
#include <memory>
#include <optional>

#include <mqueue.h>
#include <mutex>
#include <string>

namespace device_reminder {

class ProcessMessageQueue : public IProcessMessageQueue {
public:
    explicit ProcessMessageQueue(const std::string& name,
                                 bool create = false,
                                 size_t max_messages = 16,
                                 std::shared_ptr<ILogger> logger = nullptr);
    ~ProcessMessageQueue();

    bool push(const ProcessMessage& msg);
    std::optional<ProcessMessage> pop();
    bool pop(ProcessMessage& out);

    bool is_open() const noexcept;
    void close();

private:
    mqd_t       mq_{static_cast<mqd_t>(-1)};
    std::string name_;
    std::mutex  mtx_;
    bool        owner_{false};
    std::shared_ptr<ILogger> logger_;
};

} // namespace device_reminder
