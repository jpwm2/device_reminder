#include <gtest/gtest.h>

#include "infra/file_loader.hpp"

#include <fstream>

using namespace device_reminder;

TEST(FileLoaderTest, LoadIntSuccess) {
    std::ofstream ofs("/tmp/test_settings.txt");
    ofs << "value=42\n";
    ofs.close();
    FileLoader loader(nullptr);
    EXPECT_EQ(loader.load_int("/tmp/test_settings.txt", "value"), 42);
}

TEST(FileLoaderTest, MissingKeyThrows) {
    std::ofstream ofs("/tmp/test_settings2.txt");
    ofs << "a=1\n";
    ofs.close();
    FileLoader loader(nullptr);
    EXPECT_THROW(loader.load_int("/tmp/test_settings2.txt", "b"), std::runtime_error);
}
