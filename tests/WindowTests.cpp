#include <gtest/gtest.h>
#include "core/Window.hpp"
#include "core/EventBus.hpp"
#include <memory>

// NOTE: This test will attempt to open a window.
// In a CI environment without a display, this might fail.
TEST(WindowTest, InitialScaleIsCalculatedCorrectly) {
    // 1. Setup
    auto bus = std::make_shared<EventBus>();
    
    // 2. Instantiate Window
    // This calls InitWindow(1280, 720, ...) and sets logical res to 1600x900
    // Expected scale = 1280 / 1600 = 0.8
    Window window(bus);

    // 3. Verify
    // 1280 / 1600 = 0.8
    EXPECT_FLOAT_EQ(window.getScale(), 0.8f);
    
    // 4. Teardown is handled by Window destructor (CloseWindow)
}
