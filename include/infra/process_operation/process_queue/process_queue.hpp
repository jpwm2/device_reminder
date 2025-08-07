#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "infra/logger/i_logger.hpp"
#include "infra/message/message_codec/i_message_codec.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"

namespace device_reminder {

class ProcessQueue : public IProcessQueue {
public:
    ProcessQueue(std::shared_ptr<ILogger> logger,
                 std::shared_ptr<IMessageCodec> codec);

    void push(std::shared_ptr<IMessage> msg) override;
    std::shared_ptr<IMessage> pop() override;
    std::size_t size() const override;

private:
    std::queue<std::vector<uint8_t>> buffer_;
    mutable std::mutex mtx_;
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IMessageCodec> codec_;
};

} // namespace device_reminder

