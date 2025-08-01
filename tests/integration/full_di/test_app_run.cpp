#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "di/app_injector.hpp"

#define main app_entry_point
#include "src/main.cpp"
#undef main

using namespace testing;
using namespace device_reminder;

TEST(IntegrationAppRun, ProcessesUseDependencies) {
    AppBuilder builder;
    auto app = builder.build();

    ASSERT_NE(g_mock_pir_driver, nullptr);
    ASSERT_NE(g_mock_bt_driver, nullptr);
    ASSERT_NE(g_mock_file_loader, nullptr);
    ASSERT_NE(g_mock_logger, nullptr);

    // Return defaults for optional calls
    ON_CALL(*g_mock_bt_driver, scan()).WillByDefault(Return(std::vector<std::string>{}));
    ON_CALL(*g_mock_file_loader, load_string_list(_)).WillByDefault(Return(std::vector<std::string>{}));

    EXPECT_CALL(*g_mock_pir_driver, run()).Times(1);
    EXPECT_CALL(*g_mock_bt_driver, scan()).Times(1);
    EXPECT_CALL(*g_mock_file_loader, load_string_list("device_list")).Times(1);
    EXPECT_CALL(*g_mock_logger, info(_)).Times(AtLeast(1));

    app->run();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
