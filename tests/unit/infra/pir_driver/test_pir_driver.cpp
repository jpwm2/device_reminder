#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>

#include "infra/pir_driver/pir_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/gpio_operation/gpio_reader/i_gpio_reader.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/logger/i_logger.hpp"

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::NiceMock;

namespace {
class MockGPIO : public IGPIOReader {
public:
    MOCK_METHOD(bool, read, (), (override));
    MOCK_METHOD(void, poll_edge, (bool), (override));
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
    DummyLoader(int i = 0, std::string s = "") : i_(i), s_(std::move(s)) {}
    int load_int() const override { return i_; }
    std::string load_string() const override { return s_; }
    std::vector<std::string> load_string_list() const override { return {}; }
private:
    int i_;
    std::string s_;
};
} // namespace

TEST(PIRDriverTest, RunDetectsChangeAndSendsMessage) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    NiceMock<MockSender> sender;
    DummyLoader chip_loader(0, "chip");
    DummyLoader pin_loader(1);

    EXPECT_CALL(logger, info(testing::_)).Times(testing::AtLeast(1));
    EXPECT_CALL(gpio, poll_edge(true)).WillOnce(testing::Return());
    EXPECT_CALL(sender, send()).Times(1);

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&chip_loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}),
        pin_loader);

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

TEST(PIRDriverTest, RunAlreadyRunningDoesNothing) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    StrictMock<MockSender> sender;
    DummyLoader chip_loader(0, "chip");
    DummyLoader pin_loader(1);

    EXPECT_CALL(sender, send()).Times(0);
    EXPECT_CALL(gpio, poll_edge(true)).WillOnce(testing::Return());

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&chip_loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}),
        pin_loader);

    driver->run();
    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

TEST(PIRDriverTest, RunWithoutGPIODoesNotStart) {
    NiceMock<MockLogger> logger;
    StrictMock<MockSender> sender;
    DummyLoader chip_loader(0, "chip");
    DummyLoader pin_loader(1);

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&chip_loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        nullptr,
        pin_loader);

    driver->run();
    driver->stop();
}

TEST(PIRDriverTest, RunWithoutSenderStartsButNoSend) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    DummyLoader chip_loader(0, "chip");
    DummyLoader pin_loader(1);

    EXPECT_CALL(gpio, poll_edge(true)).WillOnce(testing::Return());

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&chip_loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        nullptr,
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}),
        pin_loader);

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

TEST(PIRDriverTest, RunLogsErrorWhenReadThrows) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    NiceMock<MockSender> sender;
    DummyLoader chip_loader(0, "chip");
    DummyLoader pin_loader(1);

    EXPECT_CALL(gpio, poll_edge(true)).WillOnce(testing::Throw(std::runtime_error("err")));
    EXPECT_CALL(logger, error(testing::_)).Times(testing::AtLeast(1));

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&chip_loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}),
        pin_loader);

    driver->run();
    EXPECT_THROW(driver->stop(), std::runtime_error);
}

TEST(PIRDriverTest, StopLogsMessage) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    NiceMock<MockSender> sender;
    DummyLoader chip_loader(0, "chip");
    DummyLoader pin_loader(1);

    EXPECT_CALL(gpio, poll_edge(true)).WillOnce(testing::Return());

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&chip_loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}),
        pin_loader);

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

TEST(PIRDriverTest, StopWithoutLogger) {
    StrictMock<MockGPIO> gpio;
    StrictMock<MockSender> sender;
    DummyLoader chip_loader(0, "chip");
    DummyLoader pin_loader(1);

    EXPECT_CALL(gpio, poll_edge(true)).WillOnce(testing::Return());

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&chip_loader, [](IFileLoader*){}),
        nullptr,
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}),
        pin_loader);

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    driver->stop();
}

