#pragma once
#include "core/EventBus.hpp"
#include "entities/Car.hpp"
#include "entities/map/Modules.hpp"
#include "entities/map/World.hpp"
#include <memory>
#include <vector>

/**
 * @class EntityManager
 * @brief Manages the lifecycle and storage of all game entities.
 *
 * Stores the World, Modules, and Cars.
 * Subscribes to events to trigger spawning, generation, and updates.
 */
class EntityManager {
public:
  /**
   * @brief Constructs the EntityManager.
   * @param bus EventBus for communication.
   */
  explicit EntityManager(std::shared_ptr<EventBus> bus);
  ~EntityManager();

  /**
   * @brief Updates all managed entities.
   * @param dt Delta time.
   */
  void update(double dt);

  /**
   * @brief Draws all managed entities in the correct order (World -> Modules -> Cars -> Overlay).
   */
  void draw();

  // Entity Management
  void setWorld(std::unique_ptr<World> world);
  void addModule(std::unique_ptr<Module> module);
  void addCar(std::unique_ptr<Car> car);

  // Accessors
  World *getWorld() const { return world.get(); }
  const std::vector<std::unique_ptr<Module>> &getModules() const { return modules; }
  const std::vector<std::unique_ptr<Car>> &getCars() const { return cars; }

  /**
   * @brief Clears all entities and resets the world.
   */
  void clear();

  /**
   * @brief Removes a specific car from the simulation.
   * @param car Pointer to the car to remove.
   */
  void removeCar(Car *car);

private:
  std::shared_ptr<EventBus> eventBus;
  std::vector<Subscription> eventTokens;

  std::unique_ptr<World> world;
  std::vector<std::unique_ptr<Module>> modules;
  std::vector<std::unique_ptr<Car>> cars;
};
