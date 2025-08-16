#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "human_task/human_dispatcher.hpp"
#include "infra/message/message.hpp"

using ::testing::StrictMock;

namespace device_reminder {

class MockHumanHandler : public IHumanHandler {
public:
    MOCK_METHOD(void, get_detect, (), (override));
    MOCK_METHOD(void, start_detect, (), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

TEST(HumanDispatcherIntegrationTest, HandlesAllowedSequences) {
    auto handler = std::make_shared<StrictMock<MockHumanHandler>>();
    auto logger  = std::make_shared<DummyLogger>();
    HumanDispatcher dispatcher(logger, handler, MessageType::HumanDetected);

    EXPECT_CALL(*handler, get_detect()).Times(1);
    dispatcher.dispatch(std::make_shared<Message>(MessageType::HumanDetected,
                                                  std::vector<std::string>{},
                                                  nullptr));

    EXPECT_CALL(*handler, start_detect()).Times(1);
    dispatcher.dispatch(std::make_shared<Message>(MessageType::CooldownTimeout,
                                                  std::vector<std::string>{},
                                                  nullptr));
}

TEST(HumanDispatcherIntegrationTest, UndefinedMessageTypeDoesNothing) {
    auto handler = std::make_shared<StrictMock<MockHumanHandler>>();
    auto logger  = std::make_shared<DummyLogger>();
    HumanDispatcher dispatcher(logger, handler, MessageType::HumanDetected);

    EXPECT_CALL(*handler, get_detect()).Times(0);
    EXPECT_CALL(*handler, start_detect()).Times(0);

    dispatcher.dispatch(std::make_shared<Message>(MessageType::StartBuzzing,
                                                  std::vector<std::string>{},
                                                  nullptr));
}

} // namespace device_reminder

