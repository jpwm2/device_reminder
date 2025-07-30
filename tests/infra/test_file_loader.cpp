#include <gtest/gtest.h>
#include "infra/file_loader/file_loader.hpp"
#include "infra/logger/logger.hpp"
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fstream>

using namespace device_reminder;

TEST(FileLoaderTest, LoadIntSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "buzz_duration_ms=5000\n";
    ofs.close();

    auto spd = std::make_shared<spdlog::logger>("test", std::make_shared<spdlog::sinks::basic_file_sink_mt>("/tmp/test_log.txt"));
    FileLoader loader("/tmp/test_settings.txt", std::make_shared<Logger>(spd));

    EXPECT_EQ(loader.load_int("buzz_duration_ms"), 5000);
}

TEST(FileLoaderTest, LoadIntThrowsIfMissing) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "a=1\n";
    ofs.close();

    FileLoader loader("/tmp/test_settings.txt");
    EXPECT_THROW(loader.load_int("b"), std::runtime_error);
}

TEST(FileLoaderTest, LoadStringSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "device_name=reminder\n";
    ofs.close();

    FileLoader loader("/tmp/test_settings.txt");
    EXPECT_EQ(loader.load_string("device_name"), "reminder");
}
