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
    ${CMAKE_SOURCE_DIR}/di
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${CMAKE_SOURCE_DIR}/main_task
    ${CMAKE_SOURCE_DIR}/human_task
    ${CMAKE_SOURCE_DIR}/bluetooth_task
    ${CMAKE_SOURCE_DIR}/buzzer_task
    ${SPDLOG_INCLUDE_DIR}
    ${BOOST_DI_INCLUDE_DIR}
)

# テストコードと対象ソース
add_executable(test_app
    tests/core/app/test_app.cpp
    tests/core/main_task/test_main_task.cpp
    tests/core/human_task/test_human_task.cpp
    tests/core/human_task/test_human_handler.cpp
    tests/core/human_task/test_human_process.cpp
    tests/core/main_task/test_main_handler.cpp
    tests/core/main_task/test_main_process.cpp
    tests/core/buzzer_task/test_buzzer_handler.cpp
    tests/core/buzzer_task/test_buzzer_task.cpp
    tests/core/buzzer_task/test_buzzer_process.cpp
    tests/core/bluetooth_task/test_bluetooth_handler.cpp
    tests/core/bluetooth_task/test_bluetooth_task.cpp
    tests/core/bluetooth_task/test_bluetooth_process.cpp
    tests/infra/logger/test_logger.cpp
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
    src/infra/process_operation/process_message.cpp
)

# GPIOReader tests
add_executable(test_gpio_reader
    tests/infra/gpio_operation/gpio_reader/test_gpio_reader.cpp
    src/infra/gpio_operation/gpio_reader/gpio_reader.cpp
    tests/stubs/gpiod_stub.cpp
)

# Additional infra tests
add_executable(test_infra_extra
    tests/infra/process_operation/test_process_message.cpp
    tests/infra/process_operation/test_process_receiver.cpp
    tests/infra/thread_operation/test_thread_receiver.cpp
    tests/infra/thread_operation/test_thread_sender.cpp
    tests/infra/thread_operation/test_thread_queue.cpp
    tests/infra/thread_operation/test_thread_dispatcher.cpp
    tests/infra/thread_operation/test_thread_message.cpp
    tests/infra/pir_driver/test_pir_driver.cpp
    tests/infra/timer_service/test_timer_service.cpp
    tests/infra/watch_dog/test_watch_dog.cpp
    tests/infra/process_operation/test_process_dispatcher.cpp
    tests/infra/process_operation/test_process_base.cpp
    tests/infra/gpio_operation/gpio_setter/test_gpio_setter.cpp
    tests/infra/buzzer_driver/test_buzzer_driver.cpp
    tests/infra/bluetooth_driver/test_bluetooth_driver.cpp
    tests/infra/file_loader/test_file_loader.cpp
    src/infra/logger/logger.cpp
    src/infra/thread_operation/thread_queue.cpp
    src/infra/process_operation/process_message.cpp
    src/infra/thread_operation/thread_sender.cpp
    src/infra/thread_operation/thread_receiver.cpp
    src/infra/thread_operation/thread_dispatcher.cpp
    src/infra/pir_driver/pir_driver.cpp
    src/infra/timer_service/timer_service.cpp
    src/infra/watch_dog/watch_dog.cpp
    src/infra/gpio_operation/gpio_setter/gpio_setter.cpp
    src/infra/buzzer_driver/buzzer_driver.cpp
    src/infra/process_operation/process_dispatcher.cpp
    src/infra/process_operation/process_base.cpp
    src/infra/process_operation/process_receiver.cpp
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
add_test(NAME InfraExtraTests COMMAND test_infra_extra)


# 動的ランタイム(MDd)に統一する
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
