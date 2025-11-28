#pragma once
#include "core/EventBus.hpp"
#include "core/EntityManager.hpp"
#include <memory>
#include <vector>

class TrafficSystem {
public:
    TrafficSystem(std::shared_ptr<EventBus> bus, const EntityManager& entityManager);
    ~TrafficSystem();

private:
    std::shared_ptr<EventBus> eventBus;
    const EntityManager& entityManager;
    std::vector<Subscription> eventTokens;
};
