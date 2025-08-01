#include <gtest/gtest.h>
#include "infra/bluetooth_driver/bluetooth_driver.hpp"

using namespace device_reminder;

namespace {
class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};
} // namespace

TEST(BluetoothDriverTest, ScanRunsWithoutThrow) {
    DummyLogger logger;
    BluetoothDriver driver(logger);
    EXPECT_NO_THROW({ auto res = driver.scan(); (void)res; });
}
