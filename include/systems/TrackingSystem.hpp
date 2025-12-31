#pragma once
#include "core/EventBus.hpp"
#include "entities/Car.hpp"
#include <memory>
#include <vector>

class TrackingSystem {
public:
    explicit TrackingSystem(std::shared_ptr<EventBus> bus);
    ~TrackingSystem();

    void update(double dt);

private:
    std::shared_ptr<EventBus> eventBus;
    std::vector<Subscription> eventTokens;
    
    Car* targetCar = nullptr;
    bool isTrackingActive = false;
    bool waitingForSpawn = false;

    void startTracking();
    void stopTracking();
};