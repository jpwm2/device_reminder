cmake_minimum_required(VERSION 3.10)
project(device_reminder_test)

set(CMAKE_CXX_STANDARD 17)

# GoogleTest を取得
add_subdirectory(external/googletest)

# spdlog を取得（ヘッダオンリー）
add_subdirectory(external/spdlog)
set(SPDLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/spdlog/include)

# boost.di を取得
add_subdirectory(external/di)
set(BOOST_DI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/di/include)

# ヘッダーファイルのパス
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/infra
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/di
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${SPDLOG_INCLUDE_DIR}
    ${BOOST_DI_INCLUDE_DIR}
)

# テストコードと対象ソース
add_executable(test_app
    tests/unit/core/app/test_app.cpp
    tests/unit/core/main_task/test_main_task.cpp
    tests/unit/core/human_task/test_human_task.cpp
    tests/unit/core/human_task/test_human_handler.cpp
    tests/unit/core/human_task/test_human_process.cpp
    tests/unit/core/main_task/test_main_handler.cpp
    tests/unit/core/main_task/test_main_process.cpp
    tests/unit/core/buzzer_task/test_buzzer_handler.cpp
    tests/unit/core/buzzer_task/test_buzzer_task.cpp
    tests/unit/core/buzzer_task/test_buzzer_process.cpp
    tests/unit/core/bluetooth_task/test_bluetooth_handler.cpp
    tests/unit/core/bluetooth_task/test_bluetooth_task.cpp
    tests/unit/core/bluetooth_task/test_bluetooth_process.cpp
    tests/unit/infra/logger/test_logger.cpp
    src/infra/logger/logger.cpp
    src/core/app/app.cpp
    src/core/main_task/main_task.cpp
    src/core/main_task/main_handler.cpp
    src/core/main_task/main_process.cpp
    src/core/human_task/human_task.cpp
    src/core/human_task/human_handler.cpp
    src/core/human_task/human_process.cpp
    src/infra/process_operation/process_base.cpp


    src/core/buzzer_task/buzzer_task.cpp
    src/core/buzzer_task/buzzer_handler.cpp
    src/core/buzzer_task/buzzer_process.cpp
    src/core/bluetooth_task/bluetooth_task.cpp
    src/core/bluetooth_task/bluetooth_handler.cpp
    src/core/bluetooth_task/bluetooth_process.cpp
    src/infra/message/message.cpp
)

# GPIOReader tests
add_executable(test_gpio_reader
    tests/unit/infra/gpio_operation/gpio_reader/test_gpio_reader.cpp
    src/infra/gpio_operation/gpio_reader.cpp
    tests/stubs/gpiod_stub.cpp
)

# Minimal thread dispatcher tests
add_executable(test_thread_dispatcher_only
    tests/infra/thread_operation/test_thread_dispatcher.cpp
    src/infra/thread_operation/thread_dispatcher/thread_dispatcher.cpp
    src/infra/message/message.cpp
    src/infra/logger/logger.cpp
)
target_link_libraries(test_thread_dispatcher_only gtest gmock gtest_main)
if(UNIX)
    target_link_libraries(test_thread_dispatcher_only pthread)
endif()

# Additional infra tests
add_executable(test_infra_extra
    tests/unit/infra/process_operation/test_process_receiver.cpp
    tests/unit/infra/thread_operation/test_thread_receiver.cpp
    tests/unit/infra/thread_operation/test_thread_sender.cpp
    tests/unit/infra/thread_operation/test_thread_queue.cpp
    tests/infra/thread_operation/test_thread_dispatcher.cpp
    tests/unit/infra/pir_driver/test_pir_driver.cpp
    tests/unit/infra/timer_service/test_timer_service.cpp
    tests/unit/infra/watch_dog/test_watch_dog.cpp
    tests/unit/infra/process_operation/test_process_dispatcher.cpp
    tests/unit/infra/process_operation/test_process_base.cpp
    tests/unit/infra/gpio_operation/gpio_setter/test_gpio_setter.cpp
    tests/unit/infra/buzzer_driver/test_buzzer_driver.cpp
    tests/unit/infra/bluetooth_driver/test_bluetooth_driver.cpp
    tests/unit/infra/file_loader/test_file_loader.cpp
    src/infra/logger/logger.cpp
    src/infra/thread_operation/thread_queue.cpp
    src/infra/thread_operation/thread_sender/thread_sender.cpp
    src/infra/thread_operation/thread_receiver/thread_receiver.cpp
    src/infra/thread_operation/thread_dispatcher/thread_dispatcher.cpp
    src/infra/message/message.cpp
    src/infra/pir_driver/pir_driver.cpp
    src/infra/timer_service/timer_service.cpp
    src/infra/watch_dog/watch_dog.cpp
    src/infra/gpio_operation/gpio_setter/gpio_setter.cpp
    src/infra/buzzer_driver/buzzer_driver.cpp
    src/infra/process_operation/process_dispatcher/process_dispatcher.cpp
    src/infra/process_operation/process_base.cpp
    src/infra/process_operation/process_receiver/process_receiver.cpp
    src/infra/bluetooth_driver/bluetooth_driver.cpp
    src/infra/file_loader/file_loader.cpp
    tests/stubs/gpiod_stub.cpp
    tests/stubs/posix_mq_stub.cpp
    tests/stubs/popen_stub.cpp
)


# Build application with main.cpp
file(GLOB_RECURSE DEVICE_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(device_reminder ${DEVICE_SOURCES})
if(UNIX)
    target_link_libraries(device_reminder pthread)
endif()

# Integration test sources (exclude src/main.cpp to avoid duplicate main)
set(DEVICE_SOURCES_NO_MAIN ${DEVICE_SOURCES})
list(REMOVE_ITEM DEVICE_SOURCES_NO_MAIN ${CMAKE_SOURCE_DIR}/src/main.cpp)
add_executable(test_integration
    tests/integration/test_main.cpp
    tests/integration/infra/thread_operation/thread_sender/test_sender.cpp
    ${DEVICE_SOURCES_NO_MAIN}
    tests/stubs/gpiod_stub.cpp
)
target_include_directories(test_integration BEFORE PRIVATE tests/integration)
target_link_libraries(test_integration gtest gmock pthread)

add_executable(test_integration_full
    tests/integration/full_di/test_app_run.cpp
    ${DEVICE_SOURCES_NO_MAIN}
    tests/stubs/gpiod_stub.cpp
)
target_include_directories(test_integration_full BEFORE PRIVATE tests/integration/full_di tests/integration)
target_link_libraries(test_integration_full gtest gmock pthread)

# GoogleTestライブラリをリンク
target_link_libraries(test_app
    gtest
    gmock
    gtest_main
)


target_link_libraries(test_gpio_reader
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
    target_link_libraries(test_gpio_reader pthread)
endif()

if(UNIX)
    target_link_libraries(test_infra_extra pthread rt)
endif()


enable_testing()
add_test(NAME AllTests COMMAND test_app)
add_test(NAME GPIOReaderTests COMMAND test_gpio_reader)
add_test(NAME ThreadDispatcherTests COMMAND test_thread_dispatcher_only)
add_test(NAME IntegrationMain COMMAND test_integration)
add_test(NAME IntegrationAppRun COMMAND test_integration_full)


# 動的ランタイム(MDd)に統一する
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
