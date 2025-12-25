#include "entities/map/WorldGenerator.hpp"
#include "config.hpp"
#include "core/Logger.hpp"
#include "entities/map/Modules.hpp"
#include "raymath.h"
#include <algorithm>
#include <random>
#include <vector>

/**
 * @file WorldGenerator.cpp
 * @brief Implementation of the procedural generation algorithm.
 */

struct PlannedUnit {
  std::unique_ptr<Module> road;
  std::unique_ptr<Module> topFacility;
  std::unique_ptr<Module> bottomFacility;
};

GeneratedMap WorldGenerator::generate(const MapConfig &config) {
  Logger::Info("Generating World...");

  std::vector<std::unique_ptr<Module>> modules;
  std::vector<PlannedUnit> plan;
  std::random_device rd;
  std::mt19937 gen(rd());

  int smallParkingLeft = config.smallParkingCount;
  int largeParkingLeft = config.largeParkingCount;
  int smallChargingLeft = config.smallChargingCount;
  int largeChargingLeft = config.largeChargingCount;

  auto createFacility = [&](int type, int size, bool isTop) -> std::unique_ptr<Module> {
    if (type == 0) {
      if (size == 0)
        return std::make_unique<SmallParking>(isTop);
      else
        return std::make_unique<LargeParking>(isTop);
    } else {
      if (size == 0)
        return std::make_unique<SmallChargingStation>(isTop);
      else
        return std::make_unique<LargeChargingStation>(isTop);
    }
  };

  auto getNextFacility = [&](bool isTop) -> std::unique_ptr<Module> {
    std::vector<int> available;
    if (smallParkingLeft > 0)
      available.push_back(0);
    if (largeParkingLeft > 0)
      available.push_back(1);
    if (smallChargingLeft > 0)
      available.push_back(2);
    if (largeChargingLeft > 0)
      available.push_back(3);
    if (available.empty())
      return nullptr;
    std::uniform_int_distribution<> dist(0, (int)available.size() - 1);
    int choice = available[dist(gen)];
    if (choice == 0) {
      smallParkingLeft--;
      return createFacility(0, 0, isTop);
    }
    if (choice == 1) {
      largeParkingLeft--;
      return createFacility(0, 1, isTop);
    }
    if (choice == 2) {
      smallChargingLeft--;
      return createFacility(1, 0, isTop);
    }
    if (choice == 3) {
      largeChargingLeft--;
      return createFacility(1, 1, isTop);
    }
    return nullptr;
  };

  // 1. PLAN
  while (true) {
    int totalLeft = smallParkingLeft + largeParkingLeft + smallChargingLeft + largeChargingLeft;
    if (totalLeft <= 0)
      break;
    PlannedUnit unit;
    int rType =
        (totalLeft >= 2) ? std::uniform_int_distribution<>(0, 2)(gen) : std::uniform_int_distribution<>(0, 1)(gen);
    if (rType == 0) {
      unit.road = std::make_unique<UpEntranceRoad>();
      unit.topFacility = getNextFacility(true);
    } else if (rType == 1) {
      unit.road = std::make_unique<DownEntranceRoad>();
      unit.bottomFacility = getNextFacility(false);
    } else {
      unit.road = std::make_unique<DoubleEntranceRoad>();
      unit.topFacility = getNextFacility(true);
      unit.bottomFacility = getNextFacility(false);
    }
    if (unit.topFacility)
      unit.topFacility->setParent(unit.road.get());
    if (unit.bottomFacility)
      unit.bottomFacility->setParent(unit.road.get());
    plan.push_back(std::move(unit));
  }

  // 2. PLACEMENT
  float currentX = 0.0f; // This tracks the current "connection point" X
  float startY = 50.0f;
  float safeX_top = -1e6f;
  float safeX_bottom = -1e6f;
  float safeX_road = -1e6f;

  auto placeRoadAt = [&](float &x, float y) {
    auto road = std::make_unique<NormalRoad>();
    const auto *leftAtt = road->getAttachmentPointByNormal({-1, 0});
    const auto *rightAtt = road->getAttachmentPointByNormal({1, 0});
    road->worldPosition = {x - leftAtt->position.x, y - leftAtt->position.y};
    x += (rightAtt->position.x - leftAtt->position.x);
    modules.push_back(std::move(road));
  };

  // START PADDING: One extra road inside the world at the start
  placeRoadAt(currentX, startY);
  safeX_road = currentX;

  for (auto &unit : plan) {
    bool collision = true;
    while (collision) {
      collision = false;
      const auto *roadLeft = unit.road->getAttachmentPointByNormal({-1, 0});
      Vector2 roadWorldPos = {currentX - roadLeft->position.x, startY - roadLeft->position.y};

      auto getFacLeftX = [&](const std::unique_ptr<Module> &fac, Vector2 normal) {
        if (!fac)
          return 1e9f;
        const auto *rAtt = unit.road->getAttachmentPointByNormal(normal);
        const auto *fAtt = fac->getAttachmentPointByNormal(Vector2Scale(normal, -1.0f));
        return (roadWorldPos.x + rAtt->position.x) - fAtt->position.x;
      };

      if (getFacLeftX(unit.topFacility, {0, -1}) < safeX_top ||
          getFacLeftX(unit.bottomFacility, {0, 1}) < safeX_bottom || roadWorldPos.x < safeX_road) {
        placeRoadAt(currentX, startY);
        safeX_road = currentX;
        collision = true;
      }
    }

    const auto *rL = unit.road->getAttachmentPointByNormal({-1, 0});
    unit.road->worldPosition = {currentX - rL->position.x, startY - rL->position.y};

    auto placeFac = [&](std::unique_ptr<Module> &fac, Vector2 normal, float &sideSafeX) {
      if (!fac)
        return;
      const auto *rAtt = unit.road->getAttachmentPointByNormal(normal);
      const auto *fAtt = fac->getAttachmentPointByNormal(Vector2Scale(normal, -1.0f));
      fac->worldPosition = Vector2Subtract(Vector2Add(unit.road->worldPosition, rAtt->position), fAtt->position);
      sideSafeX = fac->worldPosition.x + fac->getWidth();
      modules.push_back(std::move(fac));
    };

    placeFac(unit.topFacility, {0, -1}, safeX_top);
    placeFac(unit.bottomFacility, {0, 1}, safeX_bottom);

    const auto *rR = unit.road->getAttachmentPointByNormal({1, 0});
    currentX = unit.road->worldPosition.x + rR->position.x;
    safeX_road = currentX;
    modules.push_back(std::move(unit.road));
  }

  // 3. TAIL PADDING
  float facMaxX = currentX;
  for (const auto &mod : modules)
    facMaxX = std::max(facMaxX, mod->worldPosition.x + mod->getWidth());

  // Ensure road covers all facilities
  while (currentX < (facMaxX - 0.1f))
    placeRoadAt(currentX, startY);

  // Add the "One extra road inside" on the right
  placeRoadAt(currentX, startY);

  // 4. WORLD BOUNDS & TILE ALIGNMENT
  float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f;
  for (const auto &mod : modules) {
    minX = std::min(minX, mod->worldPosition.x);
    minY = std::min(minY, mod->worldPosition.y);
    maxX = std::max(maxX, mod->worldPosition.x + mod->getWidth());
    maxY = std::max(maxY, mod->worldPosition.y + mod->getHeight());
  }

  float tileM = (float)Config::BACKGROUND_TILE_SIZE / (float)Config::ART_PIXELS_PER_METER;

  // The "contentWidth" is what we have now.
  // We want the world to be a multiple of tileM.
  float currentWorldWidth = maxX - minX;
  float worldWidth = std::ceil(currentWorldWidth / tileM) * tileM;

  // IMPORTANT: If worldWidth > currentWorldWidth, we have a visual gap at the edge.
  // We add more roads until currentX (the road end) reaches or passes the worldWidth.
  float targetX = minX + worldWidth;
  while (currentX < (targetX - 0.1f)) {
    placeRoadAt(currentX, startY);
  }

  // Finalize world height
  float yPad = tileM * 2.0f;
  float worldHeight = std::ceil(((maxY - minY) + 2 * yPad) / tileM) * tileM;

  // 5. NORMALIZE (Shift everything so road start is exactly 0)
  // Find the first road's left attachment world X to make it 0
  float offsetX = -minX;
  float offsetY = yPad - minY;
  for (auto &mod : modules) {
    mod->worldPosition.x += offsetX;
    mod->worldPosition.y += offsetY;
  }

  // 6. EXTERNAL ROADS (Truly outside)
  float finalRoadY = startY + offsetY;

  // External Left: Connects to X=0
  auto extL = std::make_unique<NormalRoad>();
  const auto *attL = extL->getAttachmentPointByNormal({1, 0});
  extL->worldPosition = {-attL->position.x, finalRoadY - attL->position.y};
  modules.push_back(std::move(extL));

  // External Right: Connects to X=worldWidth
  auto extR = std::make_unique<NormalRoad>();
  const auto *attR = extR->getAttachmentPointByNormal({-1, 0});
  extR->worldPosition = {worldWidth - attR->position.x, finalRoadY - attR->position.y};
  modules.push_back(std::move(extR));

  auto world = std::make_unique<World>(worldWidth, worldHeight);
  return {std::move(world), std::move(modules)};
}
