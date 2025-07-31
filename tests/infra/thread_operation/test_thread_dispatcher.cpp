#include <gtest/gtest.h>
#include "infra/thread_operation/thread_dispatcher/thread_dispatcher.hpp"
#include "infra/thread_operation/thread_message/thread_message.hpp"

using namespace device_reminder;

TEST(ThreadDispatcherTest, CallsRegisteredHandler) {
    bool called = false;
    ThreadDispatcher::HandlerMap map{
        {ThreadMessageType::StartBuzzing,
         [&](std::shared_ptr<IThreadMessage>) { called = true; }}
    };
    ThreadDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StartBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_TRUE(called);
}

TEST(ThreadDispatcherTest, IgnoresUnknownMessage) {
    bool called = false;
    ThreadDispatcher::HandlerMap map{
        {ThreadMessageType::StartBuzzing,
         [&](std::shared_ptr<IThreadMessage>) { called = true; }}
    };
    ThreadDispatcher disp(nullptr, map);
    auto msg = std::make_shared<ThreadMessage>(ThreadMessageType::StopBuzzing,
                                               std::vector<std::string>{});
    disp.dispatch(msg);
    EXPECT_FALSE(called);
}
