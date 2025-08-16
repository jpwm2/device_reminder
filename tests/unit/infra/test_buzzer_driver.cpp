#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/buzzer_driver/buzzer_driver.hpp"
#include "infra/file_loader.hpp"

using namespace device_reminder;
using ::testing::StrictMock;
using ::testing::Return;

class MockGPIOSetter : public IGPIOSetter {
public:
    MOCK_METHOD(void, write, (bool), (override));
    MOCK_METHOD(void, buzz_start, (int), (override));
    MOCK_METHOD(void, buzz_stop, (), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&, const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&, const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&, const std::string&), (const, override));
};

TEST(BuzzerDriverTest, OnAndOff) {
    auto loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto gpio = std::make_shared<StrictMock<MockGPIOSetter>>();
    BuzzerDriver driver(loader, nullptr, gpio);
    EXPECT_CALL(*loader, load_string(testing::_, testing::_)).WillOnce(Return("1000"));
    EXPECT_CALL(*gpio, buzz_start(1000)).Times(1);
    EXPECT_CALL(*gpio, buzz_stop()).Times(1);
    driver.on();
    driver.off();
}
