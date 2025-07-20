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
    tests/core/test_main_task.cpp
    tests/core/test_human_task.cpp
    tests/infra/test_logger.cpp
    src/core/app.cpp
    src/core/main_task.cpp
    src/core/main_task_handler.cpp
    src/core/human_task.cpp
    src/core/human_task_handler.cpp
)

# GPIODriver tests
add_executable(test_gpio_driver
    tests/infra/test_gpio_driver.cpp
    src/infra/gpio_driver.cpp
    tests/stubs/gpiod_stub.cpp
)

# Additional infra tests
add_executable(test_infra_extra
    tests/infra/test_local_message_queue.cpp
    tests/infra/test_message_queue.cpp
    tests/infra/test_message_sender.cpp
    tests/infra/test_message_receiver.cpp
    tests/infra/test_pir_driver.cpp
    tests/infra/test_timer_service.cpp
    tests/infra/test_worker_dispatcher.cpp
    tests/infra/test_buzzer_driver.cpp
    tests/infra/test_message_codec.cpp
    src/infra/local_message_queue.cpp
    src/infra/message_operator/message_queue.cpp
    src/infra/message_operator/message_receiver.cpp
    src/infra/message_operator/message_sender.cpp
    src/infra/pir_driver.cpp
    src/infra/timer_service.cpp
    src/infra/worker_dispatcher.cpp
    tests/stubs/posix_mq_stub.cpp
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

target_link_libraries(test_infra_extra
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

if(UNIX)
    target_link_libraries(test_infra_extra pthread rt)
endif()

enable_testing()
add_test(NAME AllTests COMMAND test_app)
add_test(NAME GPIODriverTests COMMAND test_gpio_driver)
add_test(NAME InfraExtraTests COMMAND test_infra_extra)


# 動的ランタイム(MDd)に統一する
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
