#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Foo {
public:
    virtual ~Foo() = default;
    virtual int bar(int x) = 0;
};

class MockFoo : public Foo {
public:
    MOCK_METHOD(int, bar, (int), (override));
};

TEST(MockSampleTest, ReturnsMockedValue) {
    MockFoo mock;
    EXPECT_CALL(mock, bar(5)).WillOnce(::testing::Return(42));
    EXPECT_EQ(mock.bar(5), 42);
}
