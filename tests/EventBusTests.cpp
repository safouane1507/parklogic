#include <gtest/gtest.h>
#include "core/EventBus.hpp"
#include <memory>
#include <vector>

// Define some dummy events for testing
struct TestEventA { int value; };
struct TestEventB { float value; };

class EventBusTests : public ::testing::Test {
protected:
    std::shared_ptr<EventBus> bus;

    void SetUp() override {
        bus = std::make_shared<EventBus>();
    }
};

TEST_F(EventBusTests, SubscribeAndPublish) {
    bool called = false;
    int receivedValue = 0;

    auto token = bus->subscribe<TestEventA>([&](const TestEventA& e) {
        called = true;
        receivedValue = e.value;
    });

    bus->publish(TestEventA{42});

    EXPECT_TRUE(called);
    EXPECT_EQ(receivedValue, 42);
}

TEST_F(EventBusTests, MultipleSubscribers) {
    int count = 0;

    auto token1 = bus->subscribe<TestEventA>([&](const TestEventA&) { count++; });
    auto token2 = bus->subscribe<TestEventA>([&](const TestEventA&) { count++; });

    bus->publish(TestEventA{0});

    EXPECT_EQ(count, 2);
}

TEST_F(EventBusTests, MultipleEventTypes) {
    int countA = 0;
    int countB = 0;

    auto tokenA = bus->subscribe<TestEventA>([&](const TestEventA&) { countA++; });
    auto tokenB = bus->subscribe<TestEventB>([&](const TestEventB&) { countB++; });

    bus->publish(TestEventA{0});
    bus->publish(TestEventB{0.0f});

    EXPECT_EQ(countA, 1);
    EXPECT_EQ(countB, 1);
}

TEST_F(EventBusTests, UnsubscribeViaTokenDestruction) {
    int count = 0;

    {
        auto token = bus->subscribe<TestEventA>([&](const TestEventA&) { count++; });
        bus->publish(TestEventA{1});
        EXPECT_EQ(count, 1);
    } // token goes out of scope here

    bus->publish(TestEventA{2});
    EXPECT_EQ(count, 1); // Should not have incremented
}

TEST_F(EventBusTests, ManualUnsubscribe) {
    int count = 0;
    auto token = bus->subscribe<TestEventA>([&](const TestEventA&) { count++; });

    token.unsubscribe();

    bus->publish(TestEventA{0});
    EXPECT_EQ(count, 0);
}
