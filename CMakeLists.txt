cmake_minimum_required(VERSION 3.10)
project(device_reminder_test)

set(CMAKE_CXX_STANDARD 17)

# GoogleTest のサブディレクトリを追加
add_subdirectory(external/googletest)

# ヘッダーファイルのパス
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/main_process
    ${CMAKE_SOURCE_DIR}/human_process
    ${CMAKE_SOURCE_DIR}/bluetooth_process
    ${CMAKE_SOURCE_DIR}/buzzer_process
)

# app.cpp などもコンパイル対象に含める
add_executable(test_app
    tests/test_app.cpp
    src/app.cpp         # 必要に応じて他の cpp も追加
)

# GoogleTestライブラリをリンク
target_link_libraries(test_app
    gtest
    gmock
    gtest_main
)
# pthreadはUNIX系でのみリンク
if(UNIX)
    target_link_libraries(test_app pthread)
endif()

enable_testing()
add_test(NAME AllTests COMMAND test_app)


# 動的ランタイム(MDd)に統一する
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
