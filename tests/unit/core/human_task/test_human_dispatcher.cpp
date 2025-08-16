#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/human_task/human_dispatcher.hpp"
#include "infra/message/message.hpp"

namespace device_reminder {

class MockHumanHandler : public IHumanHandler {
public:
    MOCK_METHOD(void, get_detect, (), (override));
    MOCK_METHOD(void, start_detect, (), (override));
};

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

} // namespace device_reminder

using namespace device_reminder;
using ::testing::StrictMock;

TEST(HumanDispatcherTest, HumanDetectedTriggersHandler) {
    auto handler = std::make_shared<StrictMock<MockHumanHandler>>();
    HumanDispatcher dispatcher(nullptr, handler, MessageType::HumanDetected);
    EXPECT_CALL(*handler, get_detect()).Times(1);
    auto msg = std::make_shared<Message>(MessageType::HumanDetected, std::vector<std::string>{}, nullptr);
    dispatcher.dispatch(msg);
}

TEST(HumanDispatcherTest, CooldownTimeoutRestartsDetection) {
    auto handler = std::make_shared<StrictMock<MockHumanHandler>>();
    HumanDispatcher dispatcher(nullptr, handler, MessageType::HumanDetected);
    dispatcher.dispatch(std::make_shared<Message>(MessageType::StopHumanDetection, std::vector<std::string>{}, nullptr));
    EXPECT_CALL(*handler, start_detect()).Times(1);
    dispatcher.dispatch(std::make_shared<Message>(MessageType::CooldownTimeout, std::vector<std::string>{}, nullptr));
}
