#pragma once
#include "core/EntityManager.hpp"
#include "core/EventBus.hpp"
#include <memory>
#include <vector>

/**
 * @class TrafficSystem
 * @brief Manages navigation, spawning, and high-level behavior of cars.
 *
 * The TrafficSystem acts as the "Director" for the simulation. It handles:
 * - Spawning cars at intervals.
 * - Assigning parking spots and paths to cars.
 * - Monitoring car states (Parking, Exiting).
 * - Cleaning up cars that have exited the map.
 */
class TrafficSystem {
public:
  /**
   * @brief Constructs the TrafficSystem.
   * @param bus Shared pointer to the EventBus.
   * @param entityManager Reference to the EntityManager for querying modules and cars.
   */
  TrafficSystem(std::shared_ptr<EventBus> bus, const EntityManager &entityManager);
  ~TrafficSystem();

private:
  std::shared_ptr<EventBus> eventBus;
  const EntityManager &entityManager;
  std::vector<Subscription> eventTokens;

  int currentSpawnLevel = 0;
  float spawnTimer = 0.0f;

  void spawnCar();
};
