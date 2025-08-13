#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include "infra/pir_driver/pir_driver.hpp"
#include "infra/file_loader.hpp"
#include "infra/gpio_operation/gpio_reader.hpp"
#include "infra/message/thread_sender.hpp"
#include "infra/logger.hpp"

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::NiceMock;

namespace {
class MockGPIO : public IGPIOReader {
public:
    MOCK_METHOD(bool, read, (), (override));
};
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
class MockSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (), (override));
};
class DummyLoader : public IFileLoader {
public:
    int load_int(const std::string&) const override { return 0; }
    std::string load_string(const std::string&) const override { return ""; }
    std::vector<std::string> load_string_list(const std::string&) const override { return {}; }
};
} // namespace

TEST(PIRDriverTest, RunDetectsChangeAndSendsMessage) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    NiceMock<MockSender> sender;
    DummyLoader loader;

    EXPECT_CALL(logger, info(testing::_)).Times(testing::AnyNumber());

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    {
        testing::InSequence seq;
        EXPECT_CALL(gpio, read()).WillOnce(testing::Return(false));
        EXPECT_CALL(gpio, read()).WillOnce(testing::Return(true));
        EXPECT_CALL(gpio, read()).WillRepeatedly(testing::Return(true));
    }
    EXPECT_CALL(sender, send()).Times(1);

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    driver->stop();
}

TEST(PIRDriverTest, RunAlreadyRunningDoesNothing) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    StrictMock<MockSender> sender;
    DummyLoader loader;

    EXPECT_CALL(sender, send()).Times(0);
    EXPECT_CALL(gpio, read()).WillRepeatedly(testing::Return(false));

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    driver->run();
    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

TEST(PIRDriverTest, RunWithoutGPIODoesNotStart) {
    NiceMock<MockLogger> logger;
    StrictMock<MockSender> sender;
    DummyLoader loader;

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        nullptr);

    driver->run();
    driver->stop();
}

TEST(PIRDriverTest, RunWithoutSenderStartsButNoSend) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    DummyLoader loader;

    {
        testing::InSequence seq;
        EXPECT_CALL(gpio, read()).WillOnce(testing::Return(false));
        EXPECT_CALL(gpio, read()).WillOnce(testing::Return(true));
        EXPECT_CALL(gpio, read()).WillRepeatedly(testing::Return(true));
    }

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        nullptr,
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    driver->stop();
}

TEST(PIRDriverTest, RunLogsErrorWhenReadThrows) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    NiceMock<MockSender> sender;
    DummyLoader loader;

    EXPECT_CALL(gpio, read()).WillOnce(testing::Throw(std::runtime_error("err")))
                             .WillRepeatedly(testing::Return(false));

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

TEST(PIRDriverTest, StopLogsMessage) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    NiceMock<MockSender> sender;
    DummyLoader loader;

    EXPECT_CALL(gpio, read()).WillRepeatedly(testing::Return(false));

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

TEST(PIRDriverTest, StopWithoutLogger) {
    StrictMock<MockGPIO> gpio;
    StrictMock<MockSender> sender;
    DummyLoader loader;

    EXPECT_CALL(gpio, read()).WillRepeatedly(testing::Return(false));

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        nullptr,
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

