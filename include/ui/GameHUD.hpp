#pragma once
#include "core/EventBus.hpp"
#include "ui/UIManager.hpp"
#include <memory>
#include <vector>

class EntityManager;

class GameHUD {
public:
    GameHUD(std::shared_ptr<EventBus> bus, EntityManager* entityManager);
    ~GameHUD();

    void update(double dt);
    void draw();

private:
    std::shared_ptr<EventBus> eventBus;
    UIManager uiManager;
    std::vector<Subscription> eventTokens;

    bool isPaused = false;
    double currentSpeed = 1.0;
};
