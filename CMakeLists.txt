cmake_minimum_required(VERSION 3.10)
project(device_reminder_test)

set(CMAKE_CXX_STANDARD 17)

# GoogleTest のサブディレクトリを追加
add_subdirectory(external/googletest)

# ヘッダーファイルのパス
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/infra
    ${CMAKE_SOURCE_DIR}/include/infra/gpio_driver
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/main_task
    ${CMAKE_SOURCE_DIR}/human_task
    ${CMAKE_SOURCE_DIR}/bluetooth_task
    ${CMAKE_SOURCE_DIR}/buzzer_task
    ${CMAKE_SOURCE_DIR}/external/spdlog/include
)

# テストコードと対象ソース
add_executable(test_app
    tests/core/test_app.cpp
    tests/infra/test_logger.cpp
    src/core/app.cpp
)

# GPIODriver tests
add_executable(test_gpio_driver
    tests/infra/test_gpio_driver.cpp
    src/infra/gpio_driver.cpp
    tests/stubs/gpiod_stub.cpp
)

# GoogleTestライブラリをリンク
target_link_libraries(test_app
    gtest
    gmock
    gtest_main
)

target_link_libraries(test_gpio_driver
    gtest
    gmock
    gtest_main
)
# pthreadはUNIX系でのみリンク
if(UNIX)
    target_link_libraries(test_app pthread)
endif()

if(UNIX)
    target_link_libraries(test_gpio_driver pthread)
endif()

enable_testing()
add_test(NAME AllTests COMMAND test_app)
add_test(NAME GPIODriverTests COMMAND test_gpio_driver)


# 動的ランタイム(MDd)に統一する
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
