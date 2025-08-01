#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "infra/logger/i_logger.hpp"
#include "infra/file_loader/i_file_loader.hpp"
#include "infra/process_operation/process_queue/i_process_queue.hpp"
#include "infra/process_operation/process_receiver/i_process_receiver.hpp"
#include "infra/process_operation/process_sender/i_process_sender.hpp"

// 未定義の IWorkerDispatcher をテスト用に定義
class IWorkerDispatcher {
public:
    virtual ~IWorkerDispatcher() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void join() = 0;
};

using namespace device_reminder;

#include "infra/process_operation/process_base/process_base.hpp"

using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::Return;
using ::testing::Throw;

namespace {
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, info, (const std::string&), (override));
    MOCK_METHOD(void, error, (const std::string&), (override));
    MOCK_METHOD(void, warn, (const std::string&), (override));
};

class MockFileLoader : public IFileLoader {
public:
    MOCK_METHOD(int, load_int, (), (const, override));
    MOCK_METHOD(std::string, load_string, (), (const, override));
    MOCK_METHOD(std::vector<std::string>, load_string_list, (), (const, override));
};
} // namespace

TEST(ProcessBaseTest, MockNormal_PriorityFromLoader) {
    NiceMock<MockLogger> logger;
    NiceMock<MockFileLoader> loader;
    EXPECT_CALL(loader, load_int()).WillOnce(Return(5));

    ProcessBase p(nullptr, nullptr, nullptr, nullptr,
                  std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
                  std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                  "TestProcess");

    EXPECT_EQ(p.priority(), 5);
}

TEST(ProcessBaseTest, PointerAbnormal_NullFileLoader) {
    NiceMock<MockLogger> logger;

    ProcessBase p(nullptr, nullptr, nullptr, nullptr,
                  nullptr,
                  std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                  "TestProcess");

    EXPECT_EQ(p.priority(), 0);
}

TEST(ProcessBaseTest, MockAbnormal_FileLoaderThrows) {
    NiceMock<MockLogger> logger;
    StrictMock<MockFileLoader> loader;
    EXPECT_CALL(loader, load_int()).WillOnce(Throw(std::runtime_error("fail")));

    EXPECT_THROW(ProcessBase(nullptr, nullptr, nullptr, nullptr,
                             std::shared_ptr<IFileLoader>(&loader, [](IFileLoader*){}),
                             std::shared_ptr<ILogger>(&logger, [](ILogger*){}),
                             "TestProcess"),
                 std::runtime_error);
}
