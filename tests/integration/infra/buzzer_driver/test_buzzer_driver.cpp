#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/gpio_operation/gpio_setter/i_gpio_setter.hpp"

using namespace device_reminder;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::Throw;

namespace {

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockGPIO : public IGPIOSetter {
public:
    MOCK_METHOD(void, write, (bool), (override));
};

class MockLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
};

} // namespace

TEST(BuzzerDriverTest, ConstructorAllValid) {
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    EXPECT_CALL(*loader, load_int("buzz_duration_ms")).Times(1);

    BuzzerDriver driver(loader, logger, gpio);
}

TEST(BuzzerDriverTest, ConstructorLoaderNull) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);

    BuzzerDriver driver(nullptr, logger, gpio);
}

TEST(BuzzerDriverTest, ConstructorLoggerNull) {
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*loader, load_int("buzz_duration_ms")).Times(1);

    BuzzerDriver driver(loader, nullptr, gpio);
}

TEST(BuzzerDriverTest, ConstructorAllNull) {
    EXPECT_NO_THROW({ BuzzerDriver driver(nullptr, nullptr, nullptr); });
}

TEST(BuzzerDriverTest, ConstructorLoadIntThrowsLogsError) {
    auto loader = std::make_shared<StrictMock<MockLoader>>();
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    EXPECT_CALL(*loader, load_int("buzz_duration_ms")).WillOnce(Throw(std::runtime_error("err")));
    EXPECT_CALL(*logger, error("Failed to load buzzer config")).Times(1);

    BuzzerDriver driver(loader, logger, nullptr);
}

TEST(BuzzerDriverTest, OnCallsGPIOAndLogger) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    BuzzerDriver driver(nullptr, logger, gpio);

    EXPECT_CALL(*gpio, write(true)).Times(1);
    EXPECT_CALL(*logger, info("buzzer on")).Times(1);
    driver.on();
}

TEST(BuzzerDriverTest, OnWithNullGPIO) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    BuzzerDriver driver(nullptr, logger, nullptr);

    EXPECT_CALL(*logger, info("buzzer on")).Times(1);
    driver.on();
}

TEST(BuzzerDriverTest, OnWithNullLogger) {
    auto loader = std::make_shared<NiceMock<MockLoader>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    BuzzerDriver driver(loader, nullptr, gpio);

    EXPECT_CALL(*gpio, write(true)).Times(1);
    driver.on();
}

TEST(BuzzerDriverTest, OnWithAllNull) {
    BuzzerDriver driver(nullptr, nullptr, nullptr);
    EXPECT_NO_THROW(driver.on());
}

TEST(BuzzerDriverTest, OnWriteThrowsPropagates) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    BuzzerDriver driver(nullptr, logger, gpio);

    EXPECT_CALL(*gpio, write(true)).WillOnce(Throw(std::runtime_error("gpio")));
    EXPECT_CALL(*logger, info("buzzer on")).Times(0);
    EXPECT_THROW(driver.on(), std::runtime_error);
}

TEST(BuzzerDriverTest, OffCallsGPIOAndLogger) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    BuzzerDriver driver(nullptr, logger, gpio);

    EXPECT_CALL(*gpio, write(false)).Times(1);
    EXPECT_CALL(*logger, info("buzzer off")).Times(1);
    driver.off();
}

TEST(BuzzerDriverTest, OffWithNullGPIO) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    BuzzerDriver driver(nullptr, logger, nullptr);

    EXPECT_CALL(*logger, info("buzzer off")).Times(1);
    driver.off();
}

TEST(BuzzerDriverTest, OffWithNullLogger) {
    auto loader = std::make_shared<NiceMock<MockLoader>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    BuzzerDriver driver(loader, nullptr, gpio);

    EXPECT_CALL(*gpio, write(false)).Times(1);
    driver.off();
}

TEST(BuzzerDriverTest, OffWithAllNull) {
    BuzzerDriver driver(nullptr, nullptr, nullptr);
    EXPECT_NO_THROW(driver.off());
}

TEST(BuzzerDriverTest, OffWriteThrowsPropagates) {
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    auto gpio   = std::make_shared<StrictMock<MockGPIO>>();

    EXPECT_CALL(*logger, info("BuzzerDriver created")).Times(1);
    BuzzerDriver driver(nullptr, logger, gpio);

    EXPECT_CALL(*gpio, write(false)).WillOnce(Throw(std::runtime_error("gpio")));
    EXPECT_CALL(*logger, info("buzzer off")).Times(0);
    EXPECT_THROW(driver.off(), std::runtime_error);
}

