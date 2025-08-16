cmake_minimum_required(VERSION 3.10)
project(device_reminder)

set(CMAKE_CXX_STANDARD 17)

# 外部ライブラリの追加
add_subdirectory(external/googletest)
add_subdirectory(external/spdlog)
add_subdirectory(external/di)

set(SPDLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/spdlog/include)
set(BOOST_DI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/di/include)

# インクルードパス
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/infra
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/stubs
    ${SPDLOG_INCLUDE_DIR}
    ${BOOST_DI_INCLUDE_DIR}
)

# アプリケーションのソース（テストで利用する最小限）
set(DEVICE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/app/app.cpp
    ${CMAKE_SOURCE_DIR}/src/core/human_task/human_dispatcher.cpp
    ${CMAKE_SOURCE_DIR}/src/infra/message/message.cpp
)

# ユニットテスト
set(UNIT_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/tests/unit/core/app/test_app.cpp
    ${CMAKE_SOURCE_DIR}/tests/unit/core/human_task/test_human_dispatcher.cpp
)

set(STUB_SOURCES
    ${CMAKE_SOURCE_DIR}/tests/stubs/posix_mq_stub.cpp
    ${CMAKE_SOURCE_DIR}/tests/stubs/popen_stub.cpp
    ${CMAKE_SOURCE_DIR}/tests/stubs/gpiod_stub.cpp
    ${CMAKE_SOURCE_DIR}/tests/stubs/setpriority_stub.cpp
)

add_executable(test_unit
    ${UNIT_TEST_SOURCES}
    ${DEVICE_SOURCES}
    ${STUB_SOURCES}
)
target_link_libraries(test_unit gtest gmock gtest_main pthread rt)

enable_testing()
add_test(NAME unit_tests COMMAND test_unit)

# 統合テスト
#add_subdirectory(tests/integration)

# 動的ランタイム(MDd)に統一
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

