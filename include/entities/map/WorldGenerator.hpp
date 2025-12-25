#pragma once
#include "entities/map/Modules.hpp"
#include "entities/map/World.hpp"
#include "events/GameEvents.hpp"
#include <memory>
#include <vector>

/**
 * @struct GeneratedMap
 * @brief Container for the result of the map generation process.
 */
struct GeneratedMap {
  std::unique_ptr<World> world;                 ///< The generated world entity (background/grid).
  std::vector<std::unique_ptr<Module>> modules; ///< The generated road and facility modules.
};

/**
 * @class WorldGenerator
 * @brief Procedural Map Generator.
 *
 * Responsible for:
 * 1. Planning: Deciding the sequence of Roads and Facilities based on config.
 * 2. Placement: Aligning them linearly with collision avoidance.
 * 3. Padding: Adding extra roads to fill the view.
 * 4. Normalization: Centering coordinates.
 */
class WorldGenerator {
public:
  /**
   * @brief Generates a new map based on the configuration.
   * @param config The user-defined parameters (count of facilities).
   * @return A struct containing the World and Modules.
   */
  static GeneratedMap generate(const struct MapConfig &config);
};
