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

# アプリケーションのソース
file(GLOB_RECURSE DEVICE_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(device_reminder ${DEVICE_SOURCES})
if(UNIX)
    target_link_libraries(device_reminder pthread)
endif()

# ユニットテスト
file(GLOB_RECURSE UNIT_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/unit/*.cpp)

set(STUB_SOURCES
    ${CMAKE_SOURCE_DIR}/tests/stubs/posix_mq_stub.cpp
    ${CMAKE_SOURCE_DIR}/tests/stubs/popen_stub.cpp
    ${CMAKE_SOURCE_DIR}/tests/stubs/gpiod_stub.cpp
)

set(DEVICE_SOURCES_NO_MAIN ${DEVICE_SOURCES})
list(REMOVE_ITEM DEVICE_SOURCES_NO_MAIN ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(test_unit
    ${UNIT_TEST_SOURCES}
    ${DEVICE_SOURCES_NO_MAIN}
    ${STUB_SOURCES}
)
target_link_libraries(test_unit gtest gmock gtest_main pthread rt)

enable_testing()
add_test(NAME unit_tests COMMAND test_unit)

# 統合テスト
add_subdirectory(tests/integration)

# 動的ランタイム(MDd)に統一
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

