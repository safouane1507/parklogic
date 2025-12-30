#include <gtest/gtest.h>
#include "raylib.h"
#include "entities/Car.hpp"
#include "entities/map/World.hpp"
#include "entities/map/Waypoint.hpp"
#include "entities/map/Modules.hpp"   // ضروري باش نخدمو ب Modules
#include "systems/PathPlanner.hpp"    // ضروري باش نخدمو ب PathPlanner

// --- Test Suite 1: Car Logic ---

// 1. Battery & Charging Test
TEST(CarTest, BatteryLogic) {
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::ELECTRIC);

    EXPECT_FLOAT_EQ(myCar.getBatteryLevel(), 100.0f);

    // نقصو من الشارج
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

    // فالبداية وصلات (حيت ماعندهاش طريق)
    EXPECT_TRUE(myCar.hasArrived());

    // نزيدو Waypoint
    Waypoint wp({100.0f, 100.0f}); 
    myCar.addWaypoint(wp);

    // دابا ماخصهاش تكون وصلات
    EXPECT_FALSE(myCar.hasArrived());

    // نمسحو الطريق
    myCar.clearWaypoints();
    EXPECT_TRUE(myCar.hasArrived());
}

// 4. Movement Logic (Velocity Update)
TEST(CarTest, MovementLogic) {
    // نخلقو طوموبيل واقفة
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::COMBUSTION);
    
    // السرعة فالبداية 0
    EXPECT_FLOAT_EQ(myCar.getVelocity().x, 0.0f);
    EXPECT_FLOAT_EQ(myCar.getVelocity().y, 0.0f);

    // نعطيوها فيتاس (Simulate Acceleration)
    Vector2 newVel = {50.0f, 0.0f};
    myCar.setVelocity(newVel);

    // نتأكدو أن الفيتاس تبدل
    EXPECT_FLOAT_EQ(myCar.getVelocity().x, 50.0f);

    // ملاحظة: ما درناش update() هنا حيت update كتحتاج World* (nullptr يقدر يفرقعها)
    // ولكن testedنا بلي setVelocity خدامة.
}

// --- Test Suite 2: PathPlanner Logic ---

// 5. Generate Path Sanity (واش كيخرج طريق؟)
TEST(PathPlannerTest, GeneratePathSanity) {
    // 1. Setup Car
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::COMBUSTION);

    // 2. Setup Module (Large Parking Example)
    LargeParking parkingFac(true); // Top parking
    parkingFac.worldPosition = {200, 200}; // نحطوه فشي بلاصة فالعالم

    // 3. Setup Spot
    // حسب الـ Struct Spot فالكود ديالك: localPosition, orientation, id, state, price
    Spot targetSpot = { {10.0f, 10.0f}, 0.0f, 1, SpotState::FREE, 5.0f };

    // 4. Generate Path
    std::vector<Waypoint> path = PathPlanner::GeneratePath(&myCar, &parkingFac, targetSpot);

    // 5. Assertions
    EXPECT_FALSE(path.empty()) << "PathPlanner failed to generate a path!";
    EXPECT_GE(path.size(), 2) << "Path should have at least Start and End points";
}

// 6. Path Accuracy (واش الطريق كتوصل للهدف؟)
TEST(PathPlannerTest, PathEndsAtTarget) {
    Car myCar({0, 0}, nullptr, {0, 0}, Car::CarType::ELECTRIC);
    
    // نستعملو SmallChargingStation كمثال
    SmallChargingStation charger(false); // Bottom
    charger.worldPosition = {500, 500};

    Spot targetSpot = { {20.0f, 20.0f}, 1.57f, 5, SpotState::FREE, 10.0f };

    std::vector<Waypoint> path = PathPlanner::GeneratePath(&myCar, &charger, targetSpot);
    
    ASSERT_FALSE(path.empty());

    // النقطة الأخيرة فالطريق
    Waypoint finalPoint = path.back();

    // الموقع المتوقع (Global Position = Module Pos + Spot Local Pos)
    float expectedX = charger.worldPosition.x + targetSpot.localPosition.x;
    float expectedY = charger.worldPosition.y + targetSpot.localPosition.y;

    // نتأكدو بلي وصلنا (بنسبة خطأ مقبولة 5 متر)
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