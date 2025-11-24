#pragma once
#include "core/EventBus.hpp"
#include "entities/map/Modules.hpp"
#include "entities/map/World.hpp"
#include "entities/Car.hpp"
#include "scenes/IScene.hpp"
#include "ui/UIManager.hpp"
#include <memory>
#include <unordered_set>
#include <vector>

class GameScene : public IScene {
public:
  explicit GameScene(std::shared_ptr<EventBus> bus);
  ~GameScene() override;

  void load() override;
  void unload() override;
  void update(double dt) override;
  void draw() override;

  const Camera2D &getCamera() const { return camera; }
  size_t getListenerCount() const { return eventTokens.size(); }

private:
  void handleInput(double dt);

  std::shared_ptr<EventBus> eventBus;
  std::vector<Subscription> eventTokens;

  std::unique_ptr<World> world;
  std::vector<std::unique_ptr<Module>> modules;
  std::vector<std::unique_ptr<Car>> cars;

  Camera2D camera = {};
  UIManager ui;

  bool isPaused = false;
  std::unordered_set<int> keysDown;
};
