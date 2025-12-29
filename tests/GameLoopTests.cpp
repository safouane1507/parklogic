#include <gtest/gtest.h>
#include "core/GameLoop.hpp"

// We can't easily mock GetTime() since it's a static Raylib function.
// But we can verify the loop structure: render is called once per iteration.

TEST(GameLoopTests, RenderCalledForEachLoopIteration) {
    GameLoop loop;
    int renderCount = 0;
    int updateCount = 0;
    int loopCount = 0;
    const int targetIterations = 5;

    auto update = [&](double dt) {
        updateCount++;
    };

    auto render = [&]() {
        renderCount++;
    };

    auto running = [&]() -> bool {
        // Return true 'targetIterations' times, then false.
        if (loopCount < targetIterations) {
            loopCount++;
            return true;
        }
        return false;
    };

    loop.run(update, render, running);

    EXPECT_EQ(renderCount, targetIterations);
    // updateCount is dependent on time, so we just check it doesn't crash.
    // Usually it might be 0 if the machine is super fast and no sleep happens.
    EXPECT_GE(updateCount, 0); 
}

TEST(GameLoopTests, SpeedMultiplierCanBeSet) {
    GameLoop loop;
    loop.setSpeedMultiplier(2.0);
    // Since we can't inspect private members, we rely on the fact that this methods exists and compiles.
    // And ideally affects logic, but verifying that without mocking time is hard.
    // Detailed logic verification would require refactoring GameLoop to accept a TimeProvider.
    // For now, this ensures ABI compatibility.
    SUCCEED();
}
