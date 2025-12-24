#pragma once
#include "core/EventBus.hpp"
#include "ui/UIManager.hpp"
#include <memory>
#include <vector>

class GameHUD {
public:
    explicit GameHUD(std::shared_ptr<EventBus> bus);
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
