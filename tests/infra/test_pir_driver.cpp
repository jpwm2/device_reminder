#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
    int load_int() const override { return 0; }
    std::string load_string() const override { return ""; }
    std::vector<std::string> load_string_list() const override { return {}; }
};
} // namespace

TEST(PIRDriverTest, RunDetectsChangeAndSendsMessage) {
    NiceMock<MockGPIO> gpio;
    NiceMock<MockLogger> logger;
    NiceMock<MockSender> sender;
    DummyLoader loader;

    ON_CALL(gpio, read()).WillByDefault(testing::Return(true));

    auto driver = std::make_unique<PIRDriver>(
        std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
        std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
        std::shared_ptr<IThreadSender>(&sender, [](IThreadSender*){}),
        std::shared_ptr<IGPIOReader>(&gpio, [](IGPIOReader*){}));

    driver->run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    driver->stop();
}
