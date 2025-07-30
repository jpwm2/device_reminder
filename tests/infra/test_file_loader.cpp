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
    FileLoader loader(std::make_shared<Logger>(spd), "/tmp/test_settings.txt", "buzz_duration_ms");

    EXPECT_EQ(loader.load_int(), 5000);
}

TEST(FileLoaderTest, LoadIntThrowsIfMissing) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "a=1\n";
    ofs.close();

    FileLoader loader(nullptr, "/tmp/test_settings.txt", "b");
    EXPECT_THROW(loader.load_int(), std::runtime_error);
}

TEST(FileLoaderTest, LoadStringSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "device_name=reminder\n";
    ofs.close();

    FileLoader loader(nullptr, "/tmp/test_settings.txt", "device_name");
    EXPECT_EQ(loader.load_string(), "reminder");
}

TEST(FileLoaderTest, LoadStringListSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "device_list=phone,watch,tablet\n";
    ofs.close();

    FileLoader loader(nullptr, "/tmp/test_settings.txt", "device_list");
    auto list = loader.load_string_list();
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(list[0], "phone");
    EXPECT_EQ(list[1], "watch");
    EXPECT_EQ(list[2], "tablet");
}
