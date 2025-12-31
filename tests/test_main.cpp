#include <gtest/gtest.h>
#include "raylib.h"
#include "entities/Car.hpp"
#include "entities/map/World.hpp"
#include "entities/map/Waypoint.hpp"
#include "entities/map/Modules.hpp"   // Necessary to work with Modules
#include "systems/PathPlanner.hpp"    // Necessary to work with PathPlanner

// --- Test Suite 1: Car Logic ---

// 1. Battery & Charging Test
TEST(CarTest, BatteryLogic) {
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::ELECTRIC);

    EXPECT_FLOAT_EQ(myCar.getBatteryLevel(), 100.0f);

    // Decrease charge
    myCar.charge(-20.0f);
    EXPECT_FLOAT_EQ(myCar.getBatteryLevel(), 80.0f);
}

// 2. Priority Logic
TEST(CarTest, PrioritySystem) {
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::COMBUSTION);

    // Default priority
    EXPECT_EQ(myCar.getPriority(), Car::Priority::PRIORITY_DISTANCE);

    // Change priority
    myCar.setPriority(Car::Priority::PRIORITY_PRICE);
    EXPECT_EQ(myCar.getPriority(), Car::Priority::PRIORITY_PRICE);
}

// 3. Waypoint Queue Logic
TEST(CarTest, WaypointQueue) {
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::ELECTRIC);

    // Initially arrived (since it has no path)
    EXPECT_TRUE(myCar.hasArrived());

    // Add Waypoint
    Waypoint wp({100.0f, 100.0f}); 
    myCar.addWaypoint(wp);

    // Now it shouldn't have arrived
    EXPECT_FALSE(myCar.hasArrived());

    // Clear the path
    myCar.clearWaypoints();
    EXPECT_TRUE(myCar.hasArrived());
}

// 4. Movement Logic (Velocity Update)
TEST(CarTest, MovementLogic) {
    // Create a stationary car
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::COMBUSTION);
    
    // Initial velocity 0
    EXPECT_FLOAT_EQ(myCar.getVelocity().x, 0.0f);
    EXPECT_FLOAT_EQ(myCar.getVelocity().y, 0.0f);

    // Give it velocity (Simulate Acceleration)
    Vector2 newVel = {50.0f, 0.0f};
    myCar.setVelocity(newVel);

    // Ensure velocity has changed
    EXPECT_FLOAT_EQ(myCar.getVelocity().x, 50.0f);

    // Note: we didn't call update() here because update needs World* (nullptr might crash it)
    // But we tested that setVelocity works.
}

// --- Test Suite 2: PathPlanner Logic ---

// 5. Generate Path Sanity (Does it generate a path?)
TEST(PathPlannerTest, GeneratePathSanity) {
    // 1. Setup Car
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::COMBUSTION);

    // 2. Setup Module (Large Parking Example)
    LargeParking parkingFac(true); // Top parking
    parkingFac.worldPosition = {200, 200}; // Put it somewhere in the world

    // 3. Setup Spot
    // According to the Spot struct in your code: localPosition, orientation, id, state, price
    Spot targetSpot = { {10.0f, 10.0f}, 0.0f, 1, SpotState::FREE, 5.0f };

    // 4. Generate Path
    std::vector<Waypoint> path = PathPlanner::GeneratePath(&myCar, &parkingFac, targetSpot);

    // 5. Assertions
    EXPECT_FALSE(path.empty()) << "PathPlanner failed to generate a path!";
    EXPECT_GE(path.size(), 2) << "Path should have at least Start and End points";
}

// 6. Path Accuracy (Does the path reach the target?)
TEST(PathPlannerTest, PathEndsAtTarget) {
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::ELECTRIC);
    
    // Use SmallChargingStation as an example
    SmallChargingStation charger(false); // Bottom
    charger.worldPosition = {500, 500};

    Spot targetSpot = { {20.0f, 20.0f}, 1.57f, 5, SpotState::FREE, 10.0f };

    std::vector<Waypoint> path = PathPlanner::GeneratePath(&myCar, &charger, targetSpot);
    
    ASSERT_FALSE(path.empty());

    // The last point in the path
    Waypoint finalPoint = path.back();

    // Expected position (Global Position = Module Pos + Spot Local Pos)
    float expectedX = charger.worldPosition.x + targetSpot.localPosition.x;
    float expectedY = charger.worldPosition.y + targetSpot.localPosition.y;

    // Ensure we arrived (within 5 units error margin)
    EXPECT_NEAR(finalPoint.position.x, expectedX, 5.0f);
    EXPECT_NEAR(finalPoint.position.y, expectedY, 5.0f);
}

// --- Test Suite 3: World Logic ---

TEST(WorldTest, GridToggle) {
    World myWorld(1000.0f, 1000.0f);

    bool initialState = myWorld.isGridEnabled();
    myWorld.toggleGrid();
    EXPECT_NE(myWorld.isGridEnabled(), initialState);
}

// --- Main ---
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 