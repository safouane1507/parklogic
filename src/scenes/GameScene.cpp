#include "scenes/GameScene.hpp"
#include "config.hpp"
#include "core/Logger.hpp"
#include "entities/map/Modules.hpp"
#include "entities/map/WorldGenerator.hpp"
#include "events/GameEvents.hpp"
#include "events/InputEvents.hpp"
#include "ui/DebugOverlay.hpp"
#include "ui/UIButton.hpp"
#include <format>

GameScene::GameScene(std::shared_ptr<EventBus> bus, MapConfig config) : eventBus(bus), config(config) {}

GameScene::~GameScene() { Logger::Info("GameScene Destroyed"); }

void GameScene::load() {
  Logger::Info("Loading GameScene (Generated World)...");

  // Generate World
  auto generated = WorldGenerator::generate(config);
  world = std::move(generated.world);
  modules = std::move(generated.modules);

  // Setup UI
  ui.add(std::make_shared<DebugOverlay>(eventBus));

  // Spawn Car Button
  // UIButton constructor takes (pos, size, text, bus)
  // We need to set the callback separately
  auto spawnBtn = std::make_shared<UIButton>(Vector2{10, 100}, Vector2{150, 40}, "Spawn Car", eventBus);

  spawnBtn->setOnClick([this]() { eventBus->publish(SpawnCarEvent{}); });
  ui.add(spawnBtn);

  // Setup Camera
  camera.zoom = 1.0f; // Default zoom (1.0 = Config::PPM pixels per meter)
  // Center camera on the middle of the world (in meters)
  if (world) {
    camera.target = {world->getWidth() / 2.0f, world->getHeight() / 2.0f};
  } else {
    camera.target = {0, 0};
  }
  camera.offset = {Config::LOGICAL_WIDTH / 2.0f, Config::LOGICAL_HEIGHT / 2.0f};
  camera.rotation = 0.0f;

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

  eventTokens.push_back(eventBus->subscribe<CameraZoomEvent>([this](const CameraZoomEvent &e) {
    camera.zoom += e.zoomDelta;
    if (camera.zoom < 0.1f)
      camera.zoom = 0.1f;
    if (camera.zoom > 5.0f)
      camera.zoom = 5.0f;
  }));

  // 1. Handle Spawn Request
  eventTokens.push_back(eventBus->subscribe<SpawnCarEvent>([this](const SpawnCarEvent &) {
    Logger::Info("SpawnCarEvent received. Spawning car entity...");

    // Find a valid start position (Road)
    Vector2 spawnPos = {0, 0};
    for (const auto &mod : modules) {
      if (auto *r = dynamic_cast<NormalRoad *>(mod.get())) {
        std::vector<Waypoint> wps = r->getGlobalWaypoints();
        if (!wps.empty())
          spawnPos = wps[0].position;
        else
          spawnPos = r->worldPosition;
        break;
      }
    }

    auto car = std::make_unique<Car>(spawnPos, world.get());
    Car *carPtr = car.get();
    cars.push_back(std::move(car));

    // Notify that a car has spawned
    eventBus->publish(CarSpawnedEvent{carPtr});
  }));

  // 2. Handle Path Assignment (Map Logic)
  eventTokens.push_back(eventBus->subscribe<CarSpawnedEvent>([this](const CarSpawnedEvent &e) {
    Logger::Info("CarSpawnedEvent received. Calculating path...");

    std::vector<Module *> facilities;
    for (const auto &mod : modules) {
      if (dynamic_cast<SmallParking *>(mod.get()) || dynamic_cast<LargeParking *>(mod.get()) ||
          dynamic_cast<SmallChargingStation *>(mod.get()) || dynamic_cast<LargeChargingStation *>(mod.get())) {
        facilities.push_back(mod.get());
      }
    }

    if (facilities.empty()) {
      Logger::Error("No facilities found to assign path.");
      return;
    }

    // Pick random facility
    int idx = GetRandomValue(0, (int)facilities.size() - 1);
    Module *targetFac = facilities[idx];

    // Get Path from Facility (Recursive: Road -> Facility)
    std::vector<Waypoint> path = targetFac->getPath();

    // Publish Path Assignment
    eventBus->publish(AssignPathEvent{e.car, path});
  }));

  // 3. Apply Path to Car
  eventTokens.push_back(eventBus->subscribe<AssignPathEvent>([](const AssignPathEvent &e) {
    Logger::Info("AssignPathEvent received. Setting path for car.");
    if (e.car) {
      e.car->setPath(e.path);
    }
  }));
}

void GameScene::unload() {
  world.reset();
  modules.clear();
  cars.clear();
  eventTokens.clear();
}

void GameScene::handleInput(double dt) {
  // Camera Movement Speed (in meters/sec)
  // Let's say we want to move 20 meters per second
  float speed = 20.0f / camera.zoom;

  if (keysDown.contains(KEY_W))
    camera.target.y -= speed * dt;
  if (keysDown.contains(KEY_S))
    camera.target.y += speed * dt;
  if (keysDown.contains(KEY_A))
    camera.target.x -= speed * dt;
  if (keysDown.contains(KEY_D))
    camera.target.x += speed * dt;

  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    eventBus->publish(CameraZoomEvent{wheel * 0.1f});
  }

  if (camera.zoom < 0.1f)
    camera.zoom = 0.1f;
  if (camera.zoom > 5.0f)
    camera.zoom = 5.0f;

  // Clamp Camera Target to World Bounds (in meters)
  if (world) {
    float worldW = world->getWidth();
    float worldH = world->getHeight();

    if (camera.target.x < 0)
      camera.target.x = 0;
    if (camera.target.y < 0)
      camera.target.y = 0;
    if (camera.target.x > worldW)
      camera.target.x = worldW;
    if (camera.target.y > worldH)
      camera.target.y = worldH;
  }
}

void GameScene::update(double dt) {
  handleInput(dt);
  ui.update(dt);

  if (!isPaused) {
    if (world)
      world->update(dt);

    for (auto &car : cars) {
      car->updateWithNeighbors(dt, &cars);
    }
  }
}

void GameScene::draw() {
  // Create a render camera that applies the PPM scaling
  Camera2D renderCamera = camera;
  renderCamera.zoom = camera.zoom * Config::PPM;

  BeginMode2D(renderCamera);
  ClearBackground(RAYWHITE);

  if (world)
    world->draw();

  for (const auto &mod : modules) {
    mod->draw();
  }

  for (const auto &car : cars) {
    car->draw();
  }

  EndMode2D();

  if (isPaused) {
    DrawText("PAUSED", Config::LOGICAL_WIDTH / 2 - 100, 50, 60, MAROON);
  }

  DrawText("WASD: Move | Scroll: Zoom | ESC: Menu", 10, Config::LOGICAL_HEIGHT - 30, 20, DARKGRAY);

  ui.draw();
}
