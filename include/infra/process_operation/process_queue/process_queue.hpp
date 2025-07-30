#pragma once
#include <mqueue.h>
#include <mutex>
#include <string>
#include <memory>

#include "infra/logger/i_logger.hpp"
#include "infra/process_operation/message_codec/i_message_codec.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"

namespace device_reminder {

class ProcessQueue : public IProcessQueue {
public:
    ProcessQueue(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IMessageCodec> codec,
                 const std::string& queue_name);
    ~ProcessQueue();

    void push(std::shared_ptr<IProcessMessage> msg) override;
    std::shared_ptr<IProcessMessage> pop() override;
    std::size_t size() const override;

private:
    mqd_t mq_{static_cast<mqd_t>(-1)};
    std::string queue_name_;
    std::shared_ptr<IMessageCodec> codec_;
    std::shared_ptr<ILogger> logger_;
    mutable std::mutex mtx_;

    static constexpr long kMaxMsg = 10;
    static constexpr long kMsgSize = 512;
};

} // namespace device_reminder
