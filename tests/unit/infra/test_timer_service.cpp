#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/timer_service/timer_service.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"
#include "infra/message/thread_sender.hpp"

#include <thread>

using namespace device_reminder;
using ::testing::StrictMock;

class MockThreadSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (std::shared_ptr<IMessageQueue>, std::shared_ptr<IMessage>), (override));
};

TEST(TimerServiceTest, StartThrowsWithoutSender) {
    TimerService timer(nullptr);
    auto queue = std::make_shared<MessageQueue>(nullptr);
    auto msg = std::make_shared<Message>(MessageType::Timeout, std::vector<std::string>{}, nullptr);
    EXPECT_THROW(timer.start(1, nullptr, queue, msg), std::invalid_argument);
}

TEST(TimerServiceTest, StartCallsSender) {
    TimerService timer(nullptr);
    auto queue = std::make_shared<MessageQueue>(nullptr);
    auto msg = std::make_shared<Message>(MessageType::Timeout, std::vector<std::string>{}, nullptr);
    auto sender = std::make_shared<StrictMock<MockThreadSender>>();
    EXPECT_CALL(*sender, send(testing::_, testing::_)).Times(1);
    timer.start(1, sender, queue, msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    timer.stop();
}
