#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/watch_dog/watch_dog.hpp"
#include "infra/message/message_queue.hpp"
#include "infra/message/message.hpp"
#include "infra/message/thread_sender.hpp"
#include "infra/timer_service/timer_service.hpp"

using namespace device_reminder;
using ::testing::StrictMock;

class MockTimerService : public ITimerService {
public:
    MOCK_METHOD(void, start, (int, std::shared_ptr<IThreadSender>, std::shared_ptr<IMessageQueue>, std::shared_ptr<IMessage>), (override));
    MOCK_METHOD(void, stop, (), (override));
};

class DummySender : public IThreadSender {
public:
    void send(std::shared_ptr<IMessageQueue>, std::shared_ptr<IMessage>) override {}
};

TEST(WatchDogTest, StartAndStopCallTimer) {
    auto timer = std::make_shared<StrictMock<MockTimerService>>();
    auto queue = std::make_shared<MessageQueue>(nullptr);
    auto msg = std::make_shared<Message>(MessageType::Timeout, std::vector<std::string>{}, nullptr);
    auto sender = std::make_shared<DummySender>();
    WatchDog dog(timer, queue, msg, sender, nullptr, 10);
    EXPECT_CALL(*timer, start(testing::_, testing::_, testing::_, testing::_)).Times(1);
    dog.start();
    EXPECT_CALL(*timer, stop()).Times(1);
    dog.stop();
}
