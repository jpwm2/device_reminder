#include <gtest/gtest.h>

#include "infra/process/process.hpp"
#include "infra/message/message_inbox.hpp"

using namespace device_reminder;

TEST(ProcessTest, RunThrowsWhenDependenciesMissing) {
    MessageInbox inbox(nullptr, nullptr, nullptr, nullptr);
    Process proc(nullptr, nullptr, nullptr, std::move(inbox));
    EXPECT_THROW(proc.run(), std::runtime_error);
}
