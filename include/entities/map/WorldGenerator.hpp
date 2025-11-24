#pragma once
#include "entities/map/Modules.hpp"
#include "entities/map/World.hpp"
#include <memory>
#include <vector>

struct GeneratedMap {
  std::unique_ptr<World> world;
  std::vector<std::unique_ptr<Module>> modules;
};

class WorldGenerator {
public:
  static GeneratedMap generate();
};
