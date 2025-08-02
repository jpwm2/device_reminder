#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/logger.hpp"

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/details/log_msg.h>

#include <fstream>
#include <filesystem>
#include <string>

// spdlog の sink をモック化
class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg& msg), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string& pattern), (override));
    MOCK_METHOD(void, set_formatter, (std::unique_ptr<spdlog::formatter> formatter), (override));
};

using ::testing::AnyNumber;
using ::testing::StrictMock;
using ::testing::Truly;
using ::testing::_;

namespace fs = std::filesystem;

namespace {

std::shared_ptr<device_reminder::Logger> create_logger(std::shared_ptr<spdlog::sinks::sink> sink)
{
    auto spd = std::make_shared<spdlog::logger>("test", std::move(sink));
    return std::make_shared<device_reminder::Logger>(std::move(spd));
}

}

// 正常系: 正しい整数値を読み込み、エラーログが出力されない
TEST(FileLoaderIntegrationTest, LoadIntReturnsValueWithoutErrorLog)
{
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto* sink_ptr = sink.get();
    EXPECT_CALL(*sink_ptr, flush()).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_pattern(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_formatter(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, log(_)).Times(0);

    auto logger = create_logger(sink);

    fs::path path = fs::temp_directory_path() / "file_loader_normal.cfg";
    {
        std::ofstream ofs(path);
        ofs << "buzz_duration_ms=5000\n";
    }

    device_reminder::FileLoader loader(logger, path.string());
    int value = loader.load_int("buzz_duration_ms");
    EXPECT_EQ(value, 5000);

    fs::remove(path);
}

// 異常系: 不正な整数値
TEST(FileLoaderIntegrationTest, LoadIntInvalidValueThrows)
{
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto* sink_ptr = sink.get();
    EXPECT_CALL(*sink_ptr, flush()).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_pattern(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_formatter(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, log(Truly([](const spdlog::details::log_msg& msg){
        return msg.level == spdlog::level::err &&
               std::string(msg.payload.data(), msg.payload.size()).find("invalid int value for buzz_duration_ms") != std::string::npos;
    })));

    auto logger = create_logger(sink);

    fs::path path = fs::temp_directory_path() / "file_loader_invalid.cfg";
    {
        std::ofstream ofs(path);
        ofs << "buzz_duration_ms=abc\n";
    }

    device_reminder::FileLoader loader(logger, path.string());
    EXPECT_THROW(loader.load_int("buzz_duration_ms"), std::invalid_argument);

    fs::remove(path);
}

// 異常系: キー不存在
TEST(FileLoaderIntegrationTest, LoadIntKeyNotFoundThrows)
{
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto* sink_ptr = sink.get();
    EXPECT_CALL(*sink_ptr, flush()).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_pattern(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_formatter(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, log(Truly([](const spdlog::details::log_msg& msg){
        return msg.level == spdlog::level::err &&
               std::string(msg.payload.data(), msg.payload.size()).find("key not found: buzz_duration_ms") != std::string::npos;
    })));

    auto logger = create_logger(sink);

    fs::path path = fs::temp_directory_path() / "file_loader_nokey.cfg";
    {
        std::ofstream ofs(path);
        ofs << "other_key=1\n";
    }

    device_reminder::FileLoader loader(logger, path.string());
    EXPECT_THROW(loader.load_int("buzz_duration_ms"), std::runtime_error);

    fs::remove(path);
}

// 異常系: ファイル不存在
TEST(FileLoaderIntegrationTest, LoadIntFileNotFoundThrows)
{
    auto sink = std::make_shared<StrictMock<MockSink>>();
    auto* sink_ptr = sink.get();
    EXPECT_CALL(*sink_ptr, flush()).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_pattern(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, set_formatter(_)).Times(AnyNumber());
    EXPECT_CALL(*sink_ptr, log(Truly([](const spdlog::details::log_msg& msg){
        return msg.level == spdlog::level::err &&
               std::string(msg.payload.data(), msg.payload.size()).find("failed to open file") != std::string::npos;
    })));

    auto logger = create_logger(sink);

    fs::path path = fs::temp_directory_path() / "file_loader_notfound.cfg";

    device_reminder::FileLoader loader(logger, path.string());
    EXPECT_THROW(loader.load_int("buzz_duration_ms"), std::runtime_error);
}

