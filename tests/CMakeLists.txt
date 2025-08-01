cmake_minimum_required(VERSION 3.10)
project(device_reminder_tests)

# GoogleTest をローカルディレクトリから追加
add_subdirectory(
    ${CMAKE_CURRENT_LIST_DIR}/../external/googletest
    ${CMAKE_BINARY_DIR}/googletest
)

# ルートディレクトリを取得
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

include_directories(
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/include/infra
    ${PROJECT_ROOT}/tests/stubs
    ${PROJECT_ROOT}/src
    ${PROJECT_ROOT}/external/spdlog/include
)

# ProcessQueue のテスト
add_executable(process_queue_tests
    infra/process_operation/test_process_queue.cpp
    ${PROJECT_ROOT}/src/infra/process_operation/process_queue.cpp
    ${PROJECT_ROOT}/src/infra/process_operation/message_codec.cpp
    ${PROJECT_ROOT}/src/infra/process_operation/process_message.cpp
    ${PROJECT_ROOT}/src/infra/logger/logger.cpp
    ${PROJECT_ROOT}/tests/stubs/posix_mq_stub.cpp
)

# ProcessSender のテスト
add_executable(process_sender_tests
    infra/process_operation/test_process_sender.cpp
    ${PROJECT_ROOT}/src/infra/process_operation/process_sender.cpp
    ${PROJECT_ROOT}/src/infra/process_operation/process_queue.cpp
    ${PROJECT_ROOT}/src/infra/process_operation/message_codec.cpp
    ${PROJECT_ROOT}/src/infra/process_operation/process_message.cpp
    ${PROJECT_ROOT}/src/infra/logger/logger.cpp
    ${PROJECT_ROOT}/tests/stubs/posix_mq_stub.cpp
)

target_link_libraries(process_queue_tests gtest gmock gtest_main pthread)

target_link_libraries(process_sender_tests gtest gmock gtest_main pthread)

enable_testing()
add_test(NAME process_queue_tests COMMAND process_queue_tests)
add_test(NAME process_sender_tests COMMAND process_sender_tests)

