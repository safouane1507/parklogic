#include "ui/GameHUD.hpp"
#include "ui/DebugOverlay.hpp"
#include "ui/UIButton.hpp"
#include "events/GameEvents.hpp"
#include "config.hpp"
#include "raylib.h"

GameHUD::GameHUD(std::shared_ptr<EventBus> bus) : eventBus(bus) {
    // Setup UI Elements
    uiManager.add(std::make_shared<DebugOverlay>(eventBus));

    auto spawnBtn = std::make_shared<UIButton>(Vector2{10, 100}, Vector2{150, 40}, "Spawn Car", eventBus);
    spawnBtn->setOnClick([this]() { eventBus->publish(SpawnCarRequestEvent{}); });
    uiManager.add(spawnBtn);

    // Subscribe to Pause Events to toggle pause text visibility
    eventTokens.push_back(eventBus->subscribe<GamePausedEvent>([this](const GamePausedEvent&) {
        isPaused = true;
    }));

    eventTokens.push_back(eventBus->subscribe<GameResumedEvent>([this](const GameResumedEvent&) {
        isPaused = false;
    }));
}

GameHUD::~GameHUD() {
    eventTokens.clear();
}

void GameHUD::update(double dt) {
    uiManager.update(dt);
}

void GameHUD::draw() {
    uiManager.draw();

    // Draw Static HUD Text
    if (isPaused) {
        DrawText("PAUSED", Config::LOGICAL_WIDTH / 2 - 100, 50, 60, MAROON);
    }

    DrawText("WASD: Move | Scroll: Zoom | ESC: Menu", 10, Config::LOGICAL_HEIGHT - 30, 20, DARKGRAY);
}
