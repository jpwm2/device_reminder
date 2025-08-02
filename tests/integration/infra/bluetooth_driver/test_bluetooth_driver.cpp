#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spdlog/logger.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/details/log_msg.h>

#include <memory>

#include "infra/bluetooth_driver/bluetooth_driver.hpp"
#include "infra/logger/logger.hpp"
#include "popen_stub.h"

using namespace testing;
using namespace device_reminder;

namespace {
class MockSink : public spdlog::sinks::sink {
public:
    MOCK_METHOD(void, log, (const spdlog::details::log_msg&), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, set_pattern, (const std::string&), (override));
    MOCK_METHOD(void, set_formatter, (std::unique_ptr<spdlog::formatter>), (override));
};
}

TEST(BluetoothDriverIntegrationTest, ScanNormal) {
    popen_stub_reset();
    popen_stub_set_output("Scanning ...\nAA:BB\tMyPhone\n");

    auto sink = std::make_shared<StrictMock<MockSink>>();
    EXPECT_CALL(*sink, set_pattern(_)).Times(AnyNumber());
    EXPECT_CALL(*sink, set_formatter(_)).Times(AnyNumber());
    EXPECT_CALL(*sink, flush()).Times(AnyNumber());
    EXPECT_CALL(*sink, log(_))
        .WillOnce(Invoke([](const spdlog::details::log_msg& msg) {
            std::string payload(msg.payload.begin(), msg.payload.end());
            EXPECT_EQ(payload, "detected device: MyPhone");
        }));

    auto spd_logger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger{spd_logger};
    BluetoothDriver driver(logger);

    auto res = driver.scan();
    ASSERT_EQ(res.size(), 1u);
    EXPECT_EQ(res[0], "MyPhone");
}

TEST(BluetoothDriverIntegrationTest, ScanAbnormal) {
    popen_stub_reset();
    popen_stub_set_fail(1);

    auto sink = std::make_shared<StrictMock<MockSink>>();
    EXPECT_CALL(*sink, set_pattern(_)).Times(AnyNumber());
    EXPECT_CALL(*sink, set_formatter(_)).Times(AnyNumber());
    EXPECT_CALL(*sink, flush()).Times(AnyNumber());
    EXPECT_CALL(*sink, log(_))
        .WillOnce(Invoke([](const spdlog::details::log_msg& msg) {
            std::string payload(msg.payload.begin(), msg.payload.end());
            EXPECT_EQ(payload, "failed to run hcitool scan");
        }));

    auto spd_logger = std::make_shared<spdlog::logger>("test", sink);
    Logger logger{spd_logger};
    BluetoothDriver driver(logger);

    EXPECT_THROW(driver.scan(), BluetoothDriverError);
}

