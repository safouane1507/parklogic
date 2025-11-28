#include "scenes/GameScene.hpp"
#include "core/EntityManager.hpp"
#include "core/Logger.hpp"
#include "events/GameEvents.hpp"
#include "events/InputEvents.hpp"
#include "systems/CameraSystem.hpp"
#include "systems/TrafficSystem.hpp"
#include "ui/GameHUD.hpp"
#include <format>

GameScene::GameScene(std::shared_ptr<EventBus> bus, MapConfig config) : eventBus(bus), config(config) {}

GameScene::~GameScene() { Logger::Info("GameScene Destroyed"); }

void GameScene::load() {
  Logger::Info("Loading GameScene (Generated World)...");

  // Initialize Managers
  cameraSystem = std::make_unique<CameraSystem>(eventBus);
  entityManager = std::make_unique<EntityManager>(eventBus);
  trafficSystem = std::make_unique<TrafficSystem>(eventBus, *entityManager);
  gameHUD = std::make_unique<GameHUD>(eventBus);

  // Generate World via Event
  eventBus->publish(GenerateWorldEvent{config});

  // Setup Camera
  // Setup Camera
  cameraSystem->setZoom(1.0f);

  // Camera setup is now handled via WorldBoundsEvent in CameraSystem

  // Subscribe to Events
  eventTokens.push_back(eventBus->subscribe<KeyPressedEvent>([this](const KeyPressedEvent &e) {
    keysDown.insert(e.key);
    if (e.key == KEY_ESCAPE) {
      Logger::Info("Switching to MainMenu");
      eventBus->publish(SceneChangeEvent{SceneType::MainMenu, {}});
    }
    if (e.key == KEY_P) {
      if (isPaused) {
        eventBus->publish(GameResumedEvent{});
      } else {
        eventBus->publish(GamePausedEvent{});
      }
    }
  }));

  eventTokens.push_back(
      eventBus->subscribe<KeyReleasedEvent>([this](const KeyReleasedEvent &e) { keysDown.erase(e.key); }));

  eventTokens.push_back(eventBus->subscribe<GamePausedEvent>([this](const GamePausedEvent &) { isPaused = true; }));
  eventTokens.push_back(eventBus->subscribe<GameResumedEvent>([this](const GameResumedEvent &) { isPaused = false; }));

  // Camera Zoom is now handled by CameraSystem

  // 1. Handle Spawn Request
  // Handled by TrafficSystem

  // 2. Handle Path Assignment (Map Logic)
  // Handled by TrafficSystem

  // 3. Apply Path to Car
  // Handled by EntityManager
}

void GameScene::unload() {
  entityManager->clear();
  eventTokens.clear();
}

void GameScene::handleInput() {
  // Camera movement is now handled by CameraSystem

  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    eventBus->publish(CameraZoomEvent{wheel * 0.1f});
  }
}

void GameScene::update(double dt) {
  handleInput();
  gameHUD->update(dt);

  if (!isPaused) {
    eventBus->publish(GameUpdateEvent{dt});
  }
}

void GameScene::draw() {
  // Create a render camera that applies the PPM scaling
  eventBus->publish(BeginCameraEvent{});
  ClearBackground(RAYWHITE);

  eventBus->publish(DrawWorldEvent{});

  eventBus->publish(EndCameraEvent{});

  eventBus->publish(EndCameraEvent{});

  gameHUD->draw();
}
