#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "di/app_injector.hpp" // our mock injector

// Rename main from src for testing
#define main app_entry_point
#include "src/main.cpp"
#undef main

using namespace testing;
using namespace device_reminder;

TEST(IntegrationMain, TasksAreCalledThroughApp) {
    AppBuilder builder;
    auto app = builder.build();

    ASSERT_NE(g_mock_main_task, nullptr);
    ASSERT_NE(g_mock_human_task, nullptr);
    ASSERT_NE(g_mock_bluetooth_task, nullptr);
    ASSERT_NE(g_mock_buzzer_task, nullptr);
    ASSERT_NE(g_mock_logger, nullptr);

    EXPECT_CALL(*g_mock_main_task, run(_)).Times(1);
    EXPECT_CALL(*g_mock_human_task, on_detecting(_)).Times(1);
    EXPECT_CALL(*g_mock_bluetooth_task, on_waiting(_)).Times(1);
    EXPECT_CALL(*g_mock_buzzer_task, run()).Times(1);
    EXPECT_CALL(*g_mock_logger, info(_)).Times(AtLeast(1));

    app->run();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
