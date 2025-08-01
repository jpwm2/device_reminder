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
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
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

TEST(BuzzerTaskTest, ConstructorLogsWhenLoggerPresent) {
    auto driver = std::make_shared<NiceMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*logger, info("BuzzerTask created")).Times(1);
    BuzzerTask task(logger, sender, loader, driver);
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

TEST(BuzzerTaskTest, ConstructorAllowsNullLogger) {
    auto driver = std::make_shared<NiceMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();

    BuzzerTask task(nullptr, sender, loader, driver);
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

TEST(BuzzerTaskTest, StartMessageTurnsOnDriver) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(logger, sender, loader, driver);

    EXPECT_CALL(*driver, on()).Times(1);
    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);
}

TEST(BuzzerTaskTest, StopMessageIgnoredWhenWaiting) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(logger, sender, loader, driver);

    EXPECT_CALL(*driver, off()).Times(0);
    task.send_message(ThreadMessage{ThreadMessageType::StopBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

TEST(BuzzerTaskTest, StartMessageWorksWithoutDriver) {
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(logger, sender, loader, nullptr);

    task.send_message(ThreadMessage{ThreadMessageType::StartBuzzing, {}});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);
}

TEST(BuzzerTaskTest, OnWaitingAndOnBuzzingCallDriver) {
    auto driver = std::make_shared<StrictMock<MockDriver>>();
    auto sender = std::make_shared<NiceMock<MockSender>>();
    auto loader = std::make_shared<NiceMock<MockFileLoader>>();
    auto logger = std::make_shared<NiceMock<MockLogger>>();

    BuzzerTask task(logger, sender, loader, driver);

    EXPECT_CALL(*driver, on()).Times(1);
    task.on_waiting({});
    EXPECT_EQ(task.state(), BuzzerTask::State::Buzzing);

    EXPECT_CALL(*driver, off()).Times(1);
    task.on_buzzing({});
    EXPECT_EQ(task.state(), BuzzerTask::State::WaitStart);
}

} // namespace device_reminder
