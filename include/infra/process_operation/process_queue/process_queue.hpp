#pragma once
#include <mutex>
#include <queue>
#include <vector>
#include <memory>

#include "infra/logger/i_logger.hpp"
#include "infra/message/message_codec/i_message_codec.hpp"
#include "infra/message/i_message.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"

namespace device_reminder {

class ProcessQueue : public IProcessQueue {
public:
    ProcessQueue(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IMessageCodec> codec);

    void push(std::shared_ptr<IMessage> msg) override;
    std::shared_ptr<IMessage> pop() override;

private:
    std::queue<std::vector<uint8_t>> buffer_;
    std::shared_ptr<IMessageCodec> codec_;
    std::shared_ptr<ILogger> logger_;
    mutable std::mutex mtx_;
};

} // namespace device_reminder
