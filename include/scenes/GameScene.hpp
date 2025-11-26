#pragma once
#include "core/EventBus.hpp"
#include "core/EventBus.hpp"
#include "events/GameEvents.hpp"
#include "entities/map/Modules.hpp"
#include "entities/map/World.hpp"
#include "entities/Car.hpp"
#include "scenes/IScene.hpp"
#include "ui/UIManager.hpp"
#include <memory>
#include <set>
#include <vector>

class GameScene : public IScene {
public:
  explicit GameScene(std::shared_ptr<EventBus> bus, MapConfig config);
  ~GameScene() override;

  void load() override;
  void unload() override;
  void update(double dt) override;
  void draw() override;

private:
  void handleInput(double dt);

  std::shared_ptr<EventBus> eventBus;
  std::vector<Subscription> eventTokens;

  std::unique_ptr<World> world;
  std::vector<std::unique_ptr<Module>> modules;
  std::vector<std::unique_ptr<Car>> cars;

  Camera2D camera = {{0,0}, {0,0}, 0.0f, 1.0f};
  bool isPaused = false;
  UIManager ui;
  MapConfig config;
  std::set<int> keysDown;
};
