#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/bluetooth_driver/bluetooth_scanner.hpp"
#include "infra/bluetooth_driver/bluetooth_pairer.hpp"
#include "infra/bluetooth_driver/bluetooth_driver.hpp"

using namespace device_reminder;
using ::testing::Return;

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

TEST(BluetoothDriverTest, PairsWhenDeviceFound) {
    auto scanner = std::make_shared<MockScanner>();
    auto pairer = std::make_shared<MockPairer>();
    BluetoothDriver driver(scanner, pairer, nullptr);
    EXPECT_CALL(*scanner, scan()).WillOnce(Return(std::vector<BluetoothDevice>{{"AA", -40}}));
    EXPECT_CALL(*pairer, pair("AA")).WillOnce(Return(true));
    driver.run();
}
