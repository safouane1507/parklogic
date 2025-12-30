#include <gtest/gtest.h>
#include "scenes/SceneManager.hpp"
#include "core/EventBus.hpp"
#include "events/GameEvents.hpp"
#include <memory>

class SceneManagerTests : public ::testing::Test {
protected:
    std::shared_ptr<EventBus> bus;
    std::unique_ptr<SceneManager> manager;

    void SetUp() override {
        bus = std::make_shared<EventBus>();
        manager = std::make_unique<SceneManager>(bus);
    }
};

TEST_F(SceneManagerTests, StartsWithNoScene) {
    // There is no public method to check the current scene,
    // but we can ensure calling update/render doesn't crash.
    EXPECT_NO_THROW(manager->update(0.1));
    EXPECT_NO_THROW(manager->render());
}

TEST_F(SceneManagerTests, CanSetSceneToMainMenu) {
    EXPECT_NO_THROW(manager->setScene(SceneType::MainMenu));
    // Verify it doesn't crash on update
    EXPECT_NO_THROW(manager->update(0.1));
}

TEST_F(SceneManagerTests, HandlesSceneChangeEvent) {
    // Publish a change event
    bus->publish(SceneChangeEvent{SceneType::MainMenu});

    // Update needs to be called to process the queued change
    EXPECT_NO_THROW(manager->update(0.1));
    
    // Subsequent updates should work
    EXPECT_NO_THROW(manager->update(0.1));
}
