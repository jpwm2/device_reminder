# GoogleTestを自動でダウンロード
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# テストターゲット
add_executable(
    test_app
    test_main.cpp
    test_app_builder.cpp
    test_app.cpp
)
# test_app用のtarget設定の直後に追加
set_target_properties(test_app PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL"
)
target_link_libraries(
    test_app
    device_reminder
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(device_reminder_tests)
