#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/bluetooth_driver/bluetooth_driver.hpp"
#include "infra/bluetooth_driver/bluetooth_scanner.hpp"
#include "infra/bluetooth_driver/bluetooth_pairer.hpp"
#include "infra/logger.hpp"

using ::testing::Return;
using ::testing::StrictMock;

namespace device_reminder {

class MockScanner : public IBluetoothScanner {
public:
    MOCK_METHOD(std::vector<BluetoothDevice>, scan, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, wait_for_stop, (), (override));
};

class MockPairer : public IBluetoothPairer {
public:
    MOCK_METHOD(bool, pair, (const std::string&), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

TEST(BluetoothDriverTest, PairsWhenDeviceWithinThreshold) {
    auto scanner = std::make_shared<StrictMock<MockScanner>>();
    auto pairer  = std::make_shared<StrictMock<MockPairer>>();
    auto logger  = std::make_shared<DummyLogger>();

    BluetoothDriver driver(scanner, pairer, logger);

    EXPECT_CALL(*scanner, scan())
        .WillOnce(Return(std::vector<BluetoothDevice>{{"AA", -40}}));
    EXPECT_CALL(*pairer, pair("AA")).WillOnce(Return(true));

    driver.run();
}

} // namespace device_reminder

