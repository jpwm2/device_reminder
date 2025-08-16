#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/buzzer_task/buzzer_dispatcher.hpp"
#include "infra/message/message.hpp"

namespace device_reminder {

class MockBuzzerHandler : public IBuzzerHandler {
public:
    MOCK_METHOD(void, start_buzzing_and_start_timer, (), (override));
    MOCK_METHOD(void, stop_buzzing_and_stop_timer, (), (override));
    MOCK_METHOD(void, stop_buzzing, (), (override));
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

TEST(BuzzerDispatcherTest, StartBuzzingCallsHandler) {
    auto handler = std::make_shared<StrictMock<MockBuzzerHandler>>();
    auto logger  = std::make_shared<StrictMock<MockLogger>>();
    BuzzerDispatcher dispatcher(logger, handler, MessageType::StartBuzzing);

    auto msg = std::make_shared<Message>(MessageType::StartBuzzing, std::vector<std::string>{}, nullptr);
    EXPECT_CALL(*handler, start_buzzing_and_start_timer()).Times(1);
    EXPECT_CALL(*logger, info(testing::_)).Times(testing::AtLeast(1));
    dispatcher.dispatch(msg);
}

TEST(BuzzerDispatcherTest, StopBuzzingCallsHandler) {
    auto handler = std::make_shared<StrictMock<MockBuzzerHandler>>();
    BuzzerDispatcher dispatcher(nullptr, handler, MessageType::StartBuzzing);

    auto msg = std::make_shared<Message>(MessageType::StopBuzzing, std::vector<std::string>{}, nullptr);
    EXPECT_CALL(*handler, stop_buzzing_and_stop_timer()).Times(1);
    dispatcher.dispatch(msg);
}
