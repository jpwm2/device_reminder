#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/bluetooth_driver/bluetooth_driver.hpp"
#include "infra/bluetooth_driver/i_bluetooth_scanner.hpp"
#include "infra/bluetooth_driver/i_bluetooth_pairer.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/thread_operation/thread_sender/i_thread_sender.hpp"
#include "infra/logger/i_logger.hpp"

using ::testing::Return;
using ::testing::StrictMock;
using ::testing::_;

namespace device_reminder {

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (const std::string&), (const, override));
    MOCK_METHOD(std::string, load_string, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (const std::string&), (const, override));
};

class MockScanner : public IBluetoothScanner {
public:
    MOCK_METHOD(std::vector<BluetoothDevice>, scan, (), (override));
};

class MockPairer : public IBluetoothPairer {
public:
    MOCK_METHOD(bool, pair, (const std::string&), (override));
};

class MockSender : public IThreadSender {
public:
    MOCK_METHOD(void, send, (), (override));
};

class DummyLogger : public ILogger {
public:
    void info(const std::string&) override {}
    void error(const std::string&) override {}
    void warn(const std::string&) override {}
};

TEST(BluetoothDriverTest, RunFiltersPairsAndNotifies) {
    auto dev_loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto dist_loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto scanner = std::make_shared<StrictMock<MockScanner>>();
    auto pairer = std::make_shared<StrictMock<MockPairer>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();

    BluetoothDriver driver(dev_loader, dist_loader, scanner, pairer, sender, logger);

    EXPECT_CALL(*dev_loader, load_string_list("device_list"))
        .WillOnce(Return(std::vector<std::string>{"AA", "CC"}));
    EXPECT_CALL(*dist_loader, load_int("distance_threshold"))
        .WillOnce(Return(50));
    EXPECT_CALL(*scanner, scan())
        .WillOnce(Return(std::vector<BluetoothDevice>{
            {"AA", 40}, {"BB", 30}, {"CC", 60}
        }));
    EXPECT_CALL(*pairer, pair("AA")).WillOnce(Return(true));
    EXPECT_CALL(*pairer, pair("CC")).Times(0);
    EXPECT_CALL(*sender, send()).Times(1);

    driver.run();

    ASSERT_EQ(driver.paired_devices().size(), 1u);
    EXPECT_EQ(driver.paired_devices()[0], "AA");
}

TEST(BluetoothDriverTest, RunNoSuccessNoNotify) {
    auto dev_loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto dist_loader = std::make_shared<StrictMock<MockFileLoader>>();
    auto scanner = std::make_shared<StrictMock<MockScanner>>();
    auto pairer = std::make_shared<StrictMock<MockPairer>>();
    auto sender = std::make_shared<StrictMock<MockSender>>();
    auto logger = std::make_shared<DummyLogger>();

    BluetoothDriver driver(dev_loader, dist_loader, scanner, pairer, sender, logger);

    EXPECT_CALL(*dev_loader, load_string_list("device_list"))
        .WillOnce(Return(std::vector<std::string>{"AA"}));
    EXPECT_CALL(*dist_loader, load_int("distance_threshold"))
        .WillOnce(Return(50));
    EXPECT_CALL(*scanner, scan())
        .WillOnce(Return(std::vector<BluetoothDevice>{{"AA", 80}}));
    EXPECT_CALL(*pairer, pair(_)).Times(0);
    EXPECT_CALL(*sender, send()).Times(0);

    driver.run();
    EXPECT_TRUE(driver.paired_devices().empty());
}

} // namespace device_reminder
