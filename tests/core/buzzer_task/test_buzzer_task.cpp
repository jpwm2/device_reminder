#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "buzzer_task/buzzer_task.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"

using ::testing::StrictMock;
using ::testing::NiceMock;

namespace device_reminder {

class MockDriver : public IBuzzerDriver {
public:
    MOCK_METHOD(void, on, (), (override));
    MOCK_METHOD(void, off, (), (override));
};


class MockSender : public IProcessSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (), (const, override));
    MOCK_METHOD(std::string, load_string, (), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (), (const, override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

TEST(BuzzerTaskTest, StartAndTimeoutStopsBuzzer) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(logger, sender, loader, driver);

    EXPECT_CALL(*driver, on());
    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);

    EXPECT_CALL(*driver, off());
    task.send_message(ThreadMessage{ThreadMessageType::StopBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

TEST(BuzzerTaskTest, ManualStopCancelsTimer) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(logger, sender, loader, driver);

    EXPECT_CALL(*driver, on());
    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzing, {}});

    EXPECT_CALL(*driver, off());
    task.send_message(ThreadMessage{ThreadMessageType::StopBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

TEST(BuzzerTaskTest, IgnoreDuplicateStart) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(logger, sender, loader, driver);

    EXPECT_CALL(*driver, on());
    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);

    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);
}

} // namespace device_reminder
