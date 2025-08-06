#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/i_logger.hpp"

#include <fstream>

using namespace device_reminder;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
}

using ::testing::NiceMock;
using ::testing::StrictMock;

TEST(FileLoaderTest, LoadIntSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "buzz_duration_ms=5000\n";
    ofs.close();

    NiceMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/test_settings.txt",
                      "buzz_duration_ms");

    EXPECT_EQ(loader.load_int(), 5000);
}

TEST(FileLoaderTest, LoadIntThrowsIfMissing) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "a=1\n";
    ofs.close();

    StrictMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/test_settings.txt",
                      "b");
    EXPECT_CALL(logger, error(testing::HasSubstr("key not found"))).Times(1);
    EXPECT_THROW(loader.load_int(), std::runtime_error);
}

TEST(FileLoaderTest, LoadIntThrowsIfInvalid) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "buzz_duration_ms=abc\n";
    ofs.close();

    StrictMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/test_settings.txt",
                      "buzz_duration_ms");
    EXPECT_CALL(logger, error(testing::HasSubstr("invalid int value"))).Times(1);
    EXPECT_THROW(loader.load_int(), std::invalid_argument);
}

TEST(FileLoaderTest, LoadIntThrowsIfFileMissing) {
    StrictMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/does_not_exist.txt",
                      "buzz_duration_ms");
    EXPECT_CALL(logger, error(testing::HasSubstr("failed to open file"))).Times(1);
    EXPECT_THROW(loader.load_int(), std::runtime_error);
}

TEST(FileLoaderTest, LoadIntInvalidWithoutLogger) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "buzz_duration_ms=bad\n";
    ofs.close();

    FileLoader loader(nullptr, "/tmp/test_settings.txt", "buzz_duration_ms");
    EXPECT_THROW(loader.load_int(), std::invalid_argument);
}

TEST(FileLoaderTest, LoadStringSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "device_name=reminder\n";
    ofs.close();

    NiceMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/test_settings.txt",
                      "device_name");
    EXPECT_EQ(loader.load_string(), "reminder");
}

TEST(FileLoaderTest, LoadStringFileMissing) {
    StrictMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/missing.txt",
                      "device_name");
    EXPECT_CALL(logger, error(testing::HasSubstr("failed to open file"))).Times(1);
    EXPECT_THROW(loader.load_string(), std::runtime_error);
}

TEST(FileLoaderTest, LoadStringKeyMissing) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "other=1\n";
    ofs.close();

    StrictMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/test_settings.txt",
                      "device_name");
    EXPECT_CALL(logger, error(testing::HasSubstr("key not found"))).Times(1);
    EXPECT_THROW(loader.load_string(), std::runtime_error);
}

TEST(FileLoaderTest, LoadStringListSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "device_list=phone,watch,tablet\n";
    ofs.close();

    NiceMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/test_settings.txt",
                      "device_list");
    auto list = loader.load_string_list();
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], "phone");
    EXPECT_EQ(list[1], "watch");
    EXPECT_EQ(list[2], "tablet");
}

TEST(FileLoaderTest, LoadStringListKeyMissing) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "other=1\n";
    ofs.close();

    StrictMock<MockLogger> logger;
    FileLoader loader(std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                      "/tmp/test_settings.txt",
                      "device_list");
    EXPECT_CALL(logger, error(testing::HasSubstr("key not found"))).Times(1);
    EXPECT_THROW(loader.load_string_list(), std::runtime_error);
}
