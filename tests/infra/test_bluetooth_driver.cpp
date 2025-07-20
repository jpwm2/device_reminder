#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/bluetooth_driver/bluetooth_driver.hpp"

using namespace device_reminder;

namespace {

class StubScanner : public IBluetoothScanner {
public:
    std::vector<AdvertisementInfo> advs{};
    bool fail = false;
    int call_count = 0;
    std::vector<AdvertisementInfo> scan() override {
        ++call_count;
        if (fail) throw BluetoothDriverError("scan failed");
        return advs;
    }
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
};

} // namespace

TEST(BluetoothDriverTest, DistanceCalculationAndFilter) {
    auto scanner = std::make_shared<StubScanner>();
    scanner->advs.push_back({"AA:BB:CC:DD:EE:FF", -50, -59});
    scanner->advs.push_back({"11:22:33:44:55:66", -90, -59});
    BluetoothDriver driver(scanner, std::make_shared<DummyLogger>());

    auto res = driver.scan_once(1.0); // 1m 以内
    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0].addr, "AA:BB:CC:DD:EE:FF");
    EXPECT_NEAR(res[0].est_distance, 0.35, 0.05);
}

TEST(BluetoothDriverTest, ScanFailureThrows) {
    auto scanner = std::make_shared<StubScanner>();
    scanner->fail = true;
    BluetoothDriver driver(scanner);
    EXPECT_THROW(driver.scan_once(1.0), BluetoothDriverError);
}

TEST(BluetoothDriverTest, MultipleCallsUseSameScanner) {
    auto scanner = std::make_shared<StubScanner>();
    BluetoothDriver driver(scanner);
    driver.scan_once(2.0);
    driver.scan_once(2.0);
    EXPECT_EQ(scanner->call_count, 2);
}

