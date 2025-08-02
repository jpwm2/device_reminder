cmake_minimum_required(VERSION 3.10)
project(integration_tests)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

# プロジェクトルートのパスを取得
get_filename_component(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)

# GoogleTest を取得
add_subdirectory(${PROJECT_ROOT}/external/googletest ${CMAKE_BINARY_DIR}/googletest)

# インクルードパス
include_directories(
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/include/core
    ${PROJECT_ROOT}/include/infra
    ${PROJECT_ROOT}/src
    ${PROJECT_ROOT}/src/core
    ${PROJECT_ROOT}/src/infra
    ${PROJECT_ROOT}
    ${PROJECT_ROOT}/tests
    ${PROJECT_ROOT}/tests/stubs
    ${PROJECT_ROOT}/external/spdlog/include
    ${PROJECT_ROOT}/external/di/include
)

# デバイス側ソース (main.cpp を除く)
file(GLOB_RECURSE DEVICE_SOURCES CONFIGURE_DEPENDS ${PROJECT_ROOT}/src/*.cpp)
list(REMOVE_ITEM DEVICE_SOURCES ${PROJECT_ROOT}/src/main.cpp)
list(REMOVE_ITEM DEVICE_SOURCES ${PROJECT_ROOT}/src/core/app_builder/app_builder.cpp)

# テストソース (core 配下のみ)
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
)

# スタブソース（gpiod は各テストでモック化）
set(STUB_SOURCES
    ${PROJECT_ROOT}/tests/stubs/posix_mq_stub.cpp
    ${PROJECT_ROOT}/tests/stubs/popen_stub.cpp
    ${PROJECT_ROOT}/tests/stubs/gpiod_stub.cpp
)

add_executable(test_integration
    ${TEST_SOURCES}
    ${DEVICE_SOURCES}
    ${STUB_SOURCES}
)

target_link_libraries(test_integration
    gtest
    gmock
    gtest_main
    pthread
    rt
)

add_test(NAME test_integration COMMAND test_integration)
