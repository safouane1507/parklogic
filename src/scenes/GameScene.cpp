#include "scenes/GameScene.hpp"
#include "config.hpp"
#include "core/Logger.hpp"
#include "entities/map/WorldGenerator.hpp"
#include "events/GameEvents.hpp"
#include "events/InputEvents.hpp"
#include <format>

GameScene::GameScene(std::shared_ptr<EventBus> bus) : eventBus(bus) {}

GameScene::~GameScene() { Logger::Info("GameScene Destroyed"); }

void GameScene::load() {
  Logger::Info("Loading GameScene (Generated World)...");

  // Generate World
  auto generated = WorldGenerator::generate();
  world = std::move(generated.world);
  modules = std::move(generated.modules);

  // Setup Camera
  camera.zoom = 1.0f; // Default zoom
  // Center camera on the middle of the world (in pixels)
  if (world) {
    camera.target = {(world->getWidth() * Config::PPM) / 2.0f, (world->getHeight() * Config::PPM) / 2.0f};
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
      eventBus->publish(SceneChangeEvent{SceneType::MainMenu});
    }
    if (e.key == KEY_P) {
      isPaused = !isPaused;
    }
  }));

  eventTokens.push_back(
      eventBus->subscribe<KeyReleasedEvent>([this](const KeyReleasedEvent &e) { keysDown.erase(e.key); }));
}

void GameScene::unload() {
  world.reset();
  modules.clear();
  eventTokens.clear();
}

void GameScene::handleInput(double dt) {
  // Camera Movement Speed (in pixels/sec)
  // Let's say we want to move 20 meters per second
  float speed = 20.0f * Config::PPM / camera.zoom;

  if (keysDown.contains(KEY_W))
    camera.target.y -= speed * dt;
  if (keysDown.contains(KEY_S))
    camera.target.y += speed * dt;
  if (keysDown.contains(KEY_A))
    camera.target.x -= speed * dt;
  if (keysDown.contains(KEY_D))
    camera.target.x += speed * dt;

  float wheel = GetMouseWheelMove();
  if (wheel != 0)
    camera.zoom += wheel * 0.1f;

  if (camera.zoom < 0.1f)
    camera.zoom = 0.1f;
  if (camera.zoom > 5.0f)
    camera.zoom = 5.0f;

  // Clamp Camera Target to World Bounds (in pixels)
  if (world) {
    float worldW = world->getWidth() * Config::PPM;
    float worldH = world->getHeight() * Config::PPM;

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
    // Update modules if they had update logic
  }
}

void GameScene::draw() {
  BeginMode2D(camera);
  ClearBackground(RAYWHITE);

  if (world)
    world->draw();

  for (const auto &mod : modules) {
    mod->draw();
  }

  EndMode2D();

  if (isPaused) {
    DrawText("PAUSED", Config::LOGICAL_WIDTH / 2 - 100, 50, 60, MAROON);
  }

  DrawText("WASD: Move | Scroll: Zoom | ESC: Menu", 10, Config::LOGICAL_HEIGHT - 30, 20, DARKGRAY);

  ui.draw();
}
