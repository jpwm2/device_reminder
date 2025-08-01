#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#include "infra/bluetooth_driver/bluetooth_driver.hpp"
#undef private
#include "popen_stub.h"

using namespace device_reminder;
using ::testing::NiceMock;
using ::testing::StrictMock;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};
class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};
} // namespace

TEST(BluetoothDriverTest, PointerArgNormal) {
    popen_stub_reset();
    popen_stub_set_output("Scanning ...\nAA:BB\tdev\n");
    DummyLogger logger;
    BluetoothDriver driver(logger);
    EXPECT_NO_THROW({ auto r = driver.scan(); (void)r; });
}

TEST(BluetoothDriverTest, ValueArgNormal) {
    popen_stub_reset();
    popen_stub_set_output("Scanning ...\nAA:BB\tphone\n");
    DummyLogger logger;
    BluetoothDriver driver(logger);
    auto res = driver.scan();
    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0], "phone");
}

TEST(BluetoothDriverTest, ValueArgAbnormal) {
    popen_stub_reset();
    popen_stub_set_output("Scanning ...\n\ninvalid\n");
    DummyLogger logger;
    BluetoothDriver driver(logger);
    auto res = driver.scan();
    EXPECT_TRUE(res.empty());
}

TEST(BluetoothDriverTest, PointerArgAbnormal) {
    popen_stub_reset();
    popen_stub_set_output("Scanning ...\nAA:BB\tdev\n");
    NiceMock<MockLogger> logger;
    BluetoothDriver driver(logger);
    driver.logger_ = nullptr;
    auto res = driver.scan();
    EXPECT_EQ(res.size(), 1u);
}

TEST(BluetoothDriverTest, MockArgNormal) {
    popen_stub_reset();
    popen_stub_set_output("Scanning ...\n11:22\tdev1\n22:33\tdev2\n");
    StrictMock<MockLogger> logger;
    EXPECT_CALL(logger, info(testing::HasSubstr("dev1"))).Times(1);
    EXPECT_CALL(logger, info(testing::HasSubstr("dev2"))).Times(1);
    BluetoothDriver driver(logger);
    auto res = driver.scan();
    EXPECT_EQ(res.size(), 2u);
}

TEST(BluetoothDriverTest, MockArgAbnormal) {
    popen_stub_reset();
    popen_stub_set_fail(1);
    StrictMock<MockLogger> logger;
    EXPECT_CALL(logger, error(testing::HasSubstr("hcitool"))).Times(1);
    BluetoothDriver driver(logger);
    EXPECT_THROW(driver.scan(), BluetoothDriverError);
}

