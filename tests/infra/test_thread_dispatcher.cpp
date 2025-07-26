#include <gtest/gtest.h>
#include "thread_message_operation/thread_dispatcher.hpp"

using namespace device_reminder;

TEST(ThreadDispatcherTest, CallsRegisteredHandler) {
    bool called = false;
    ThreadDispatcher::HandlerMap map{
        {MessageType::BuzzerOn, [&](const ThreadMessage&){ called = true; }}
    };
    ThreadDispatcher disp(nullptr, map);
    disp.dispatch(ThreadMessage{MessageType::BuzzerOn, true});
    EXPECT_TRUE(called);
}

TEST(ThreadDispatcherTest, IgnoresUnknownMessage) {
    bool called = false;
    ThreadDispatcher::HandlerMap map{
        {MessageType::BuzzerOn, [&](const ThreadMessage&){ called = true; }}
    };
    ThreadDispatcher disp(nullptr, map);
    disp.dispatch(ThreadMessage{MessageType::BuzzerOff, false});
    EXPECT_FALSE(called);
}
