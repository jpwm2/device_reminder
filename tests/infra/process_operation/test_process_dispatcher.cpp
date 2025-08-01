#include <gtest/gtest.h>

#include "infra/process_operation/process_dispatcher/process_dispatcher.hpp"
#include "infra/process_operation/process_message/process_message.hpp"

using namespace device_reminder;

TEST(ProcessDispatcherTest, CallsRegisteredHandler) {
    bool called = false;
    ProcessDispatcher::HandlerMap map{
        {ProcessMessageType::StartBuzzing,
         [&](std::shared_ptr<IProcessMessage>) { called = true; }}
    };
    ProcessDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StartBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_TRUE(called);
}

TEST(ProcessDispatcherTest, IgnoresUnknownMessage) {
    bool called = false;
    ProcessDispatcher::HandlerMap map{
        {ProcessMessageType::StartBuzzing,
         [&](std::shared_ptr<IProcessMessage>) { called = true; }}
    };
    ProcessDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ProcessMessage>(ProcessMessageType::StopBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_FALSE(called);
}

