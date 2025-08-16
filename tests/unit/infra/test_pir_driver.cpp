#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/pir_driver/pir_driver.hpp"

using namespace device_reminder;
using ::testing::Return;

class MockGPIOReader : public IGPIOReader {
public:
    MOCK_METHOD(bool, read, (), (override));
    MOCK_METHOD(void, poll_edge, (bool), (override));
};

TEST(PIRDriverTest, RunExitsIfAlreadyHigh) {
    auto gpio = std::make_shared<MockGPIOReader>();
    EXPECT_CALL(*gpio, read()).WillOnce(Return(true));
    PIRDriver driver(nullptr, nullptr, gpio);
    driver.run();
}
