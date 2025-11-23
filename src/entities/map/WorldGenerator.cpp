#include "entities/map/WorldGenerator.hpp"
#include "core/Logger.hpp"
#include "entities/map/Modules.hpp"
#include <algorithm>
#include <random>
#include <vector>

struct PlannedUnit {
  std::unique_ptr<Module> road;
  std::unique_ptr<Module> topFacility;
  std::unique_ptr<Module> bottomFacility;
};

GeneratedMap WorldGenerator::generate() {
  Logger::Info("Generating World...");

  std::vector<std::unique_ptr<Module>> modules;
  std::vector<PlannedUnit> plan;

  std::random_device rd;
  std::mt19937 gen(rd());

  // Configuration
  std::uniform_int_distribution<> numFacilitiesDist(5, 12);
  std::uniform_int_distribution<> roadTypeDist(0, 2);     // 0: Up, 1: Down, 2: Double
  std::uniform_int_distribution<> facilityTypeDist(0, 1); // 0: Parking, 1: Charging
  std::uniform_int_distribution<> sizeDist(0, 1);         // 0: Small, 1: Large

  int numFacilities = numFacilitiesDist(gen);

  auto createFacility = [&](int type, int size) -> std::unique_ptr<Module> {
    if (type == 0) { // Parking
      if (size == 0)
        return std::make_unique<SmallParking>();
      else
        return std::make_unique<LargeParking>();
    } else { // Charging
      if (size == 0)
        return std::make_unique<SmallChargingStation>();
      else
        return std::make_unique<LargeChargingStation>();
    }
  };

  for (int i = 0; i < numFacilities; ++i) {
    PlannedUnit unit;
    int rType = roadTypeDist(gen);

    if (rType == 0) { // Up
      unit.road = std::make_unique<UpEntranceRoad>();
      unit.topFacility = createFacility(facilityTypeDist(gen), sizeDist(gen));
    } else if (rType == 1) { // Down
      unit.road = std::make_unique<DownEntranceRoad>();
      unit.bottomFacility = createFacility(facilityTypeDist(gen), sizeDist(gen));
    } else { // Double
      unit.road = std::make_unique<DoubleEntranceRoad>();
      unit.topFacility = createFacility(facilityTypeDist(gen), sizeDist(gen));
      unit.bottomFacility = createFacility(facilityTypeDist(gen), sizeDist(gen));
    }
    plan.push_back(std::move(unit));
  }

  // Placement Logic
  float currentX = 0.0f;
  float startY = 50.0f;   // Middle of map
  float safeX = -1000.0f; // Rightmost edge of occupied space
  float roadWidth = 10.0f;

  // Add initial padding roads
  for (int i = 0; i < 3; ++i) {
    auto road = std::make_unique<NormalRoad>();
    road->worldPosition = {currentX, startY};
    modules.push_back(std::move(road));
    currentX += roadWidth;
  }
  safeX = currentX;

  for (auto &unit : plan) {
    // Calculate Overhangs
    float leftOverhang = 0.0f;
    float rightOverhang = 0.0f;
    float centerOffset = 5.0f; // Center of road (attachment point X)

    auto checkOverhang = [&](const std::unique_ptr<Module> &fac) {
      if (!fac)
        return;
      float w = fac->getWidth();
      // Left edge relative to road start: 5 - w/2
      // Right edge relative to road start: 5 + w/2
      float localLeft = centerOffset - w / 2.0f;
      float localRight = centerOffset + w / 2.0f;

      if (localLeft < 0)
        leftOverhang = std::max(leftOverhang, -localLeft);
      if (localRight > roadWidth)
        rightOverhang = std::max(rightOverhang, localRight - roadWidth);
    };

    checkOverhang(unit.topFacility);
    checkOverhang(unit.bottomFacility);

    // Ensure space
    // We need: currentX - leftOverhang >= safeX
    // Or: currentX >= safeX + leftOverhang
    while (currentX < safeX + leftOverhang) {
      auto road = std::make_unique<NormalRoad>();
      road->worldPosition = {currentX, startY};
      modules.push_back(std::move(road));
      currentX += roadWidth;
    }

    // Place Unit
    unit.road->worldPosition = {currentX, startY};

    if (unit.topFacility) {
      // Top facility attaches at its bottom-center to road's top-center
      // Road Top Center: {currentX + 5, startY}
      // Facility Pos: {currentX + 5 - w/2, startY - h}
      float w = unit.topFacility->getWidth();
      float h = unit.topFacility->getHeight();
      unit.topFacility->worldPosition = {currentX + 5.0f - w / 2.0f, startY - h};
      modules.push_back(std::move(unit.topFacility));
    }

    if (unit.bottomFacility) {
      // Bottom facility attaches at its top-center to road's bottom-center
      // Road Bottom Center: {currentX + 5, startY + 7.5}
      // Facility Pos: {currentX + 5 - w/2, startY + 7.5}
      float w = unit.bottomFacility->getWidth();
      // float h = unit.bottomFacility->getHeight();
      unit.bottomFacility->worldPosition = {currentX + 5.0f - w / 2.0f, startY + 7.5f};
      modules.push_back(std::move(unit.bottomFacility));
    }

    modules.push_back(std::move(unit.road));

    // Update safeX
    // safeX is the absolute X coordinate of the rightmost edge of this unit
    safeX = currentX + roadWidth + rightOverhang;

    // Advance
    currentX += roadWidth;
  }

  // Add end padding roads
  for (int i = 0; i < 3; ++i) {
    // Check if we need to space out from the last facility
    while (currentX < safeX) {
      auto road = std::make_unique<NormalRoad>();
      road->worldPosition = {currentX, startY};
      modules.push_back(std::move(road));
      currentX += roadWidth;
    }

    auto road = std::make_unique<NormalRoad>();
    road->worldPosition = {currentX, startY};
    modules.push_back(std::move(road));
    currentX += roadWidth;
    safeX = currentX;
  }

  // Calculate World Bounds
  float minX = std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::lowest();
  float maxY = std::numeric_limits<float>::lowest();

  for (const auto &mod : modules) {
    if (mod->worldPosition.x < minX)
      minX = mod->worldPosition.x;
    if (mod->worldPosition.y < minY)
      minY = mod->worldPosition.y;
    if (mod->worldPosition.x + mod->getWidth() > maxX)
      maxX = mod->worldPosition.x + mod->getWidth();
    if (mod->worldPosition.y + mod->getHeight() > maxY)
      maxY = mod->worldPosition.y + mod->getHeight();
  }

  float padding = 20.0f;
  float contentWidth = maxX - minX;
  float contentHeight = maxY - minY;

  float worldWidth = contentWidth + 2 * padding;
  float worldHeight = contentHeight + 2 * padding;

  // Shift modules to center (which means starting at padding)
  // We want minX to be at 'padding', minY to be at 'padding'
  float offsetX = padding - minX;
  float offsetY = padding - minY;

  for (auto &mod : modules) {
    mod->worldPosition.x += offsetX;
    mod->worldPosition.y += offsetY;
  }

  auto world = std::make_unique<World>(worldWidth, worldHeight);

  return {std::move(world), std::move(modules)};
}
