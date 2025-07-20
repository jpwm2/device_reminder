#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/buzzer_driver/buzzer_driver.hpp"

using namespace device_reminder;
using ::testing::StrictMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
};
} // namespace

TEST(BuzzerDriverTest, StartStopLogAndOutput) {
    testing::internal::CaptureStdout();
    auto logger = std::make_shared<StrictMock<MockLogger>>();
    BuzzerDriver driver(logger);
    EXPECT_CALL(*logger, info(testing::HasSubstr("Start Buzzer"))).Times(1);
    driver.start_buzzer();
    EXPECT_CALL(*logger, info(testing::HasSubstr("Stop Buzzer"))).Times(1);
    driver.stop_buzzer();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("Start Buzzer"));
    EXPECT_THAT(output, testing::HasSubstr("Stop Buzzer"));
}
