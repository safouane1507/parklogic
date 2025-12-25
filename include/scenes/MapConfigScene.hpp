#pragma once
#include "events/GameEvents.hpp"
#include "scenes/IScene.hpp"
#include "ui/UIManager.hpp"

class MapConfigScene : public IScene {
public:
  explicit MapConfigScene(std::shared_ptr<EventBus> bus);

  void load() override;
  void unload() override;
  void update(double dt) override;
  void draw() override;

private:
  std::shared_ptr<EventBus> eventBus;
  UIManager ui;
  MapConfig config;
};
