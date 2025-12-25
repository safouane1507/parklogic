#pragma once

/**
 * @file Modules.hpp
 * @brief Defines the building blocks of the game map (Roads, Parking, Charging).
 */
#include "entities/map/Waypoint.hpp"
#include "raylib.h"
#include <vector>

/**
 * @struct AttachmentPoint
 * @brief Defines a connection point on a module.
 */
struct AttachmentPoint {
  Vector2 position; ///< Position relative to module's top-left corner.
  Vector2 normal;   ///< Direction of the connection (e.g., {1,0} for Right).
};

enum class Lane { UP, DOWN };

/**
 * @enum ModuleType
 * @brief Categorization of modules for AI and Rendering.
 */
enum class ModuleType { GENERIC, ROAD, SMALL_PARKING, LARGE_PARKING, SMALL_CHARGING, LARGE_CHARGING };

enum class SpotState { FREE, RESERVED, OCCUPIED };

/**
 * @struct Spot
 * @brief Represents a single parking or charging space.
 */
struct Spot {
  Vector2 localPosition; ///< Position relative to facility top-left.
  float orientation;     ///< Heading angle (radians) for the car when parked.
  int id;                ///< Unique ID within the facility.
  SpotState state = SpotState::FREE;
  float price = 0.0f; ///< Dynamic price for using this spot.
};

/**
 * @class Module
 * @brief Base class for all buildable map units (Roads, Facilities).
 *
 * A Module has:
 * - Dimensions (Width/Height)
 * - Attachment Points for connecting to other modules.
 * - Local Waypoints for AI navigation.
 * - Spots (iff it's a facility).
 * - A hierarchy (Roads are parents to Facilities).
 */
class Module {
public:
  Module(float w, float h);
  virtual ~Module() = default;

  // --- Dimensions ---
  float getWidth() const { return width; }
  float getHeight() const { return height; }

  // --- Economics ---
  /**
   * @brief Gets the price multiplier for this facility.
   * @return Multiplier (e.g., 2.0 = expensive).
   */
  float getPriceMultiplier() const { return priceMultiplier; }
  void setPriceMultiplier(float m) { priceMultiplier = m; }

  /**
   * @brief Generates random prices for spots based on facility multiplier.
   * @param baseSpotPrice Base cost.
   * @param variance Random fluctuation range.
   */
  void assignRandomPricesToSpots(float baseSpotPrice, float variance);

  // --- Attachments ---
  const std::vector<AttachmentPoint> &getAttachmentPoints() const { return attachmentPoints; }

  // --- Rendering ---
  Vector2 worldPosition = {0, 0}; ///< Top-left position in the World (Meters).

  /**
   * @brief Draws the module using specific logic per type.
   */
  virtual void draw() const;

  // --- Pathfinding & Waypoints ---
  /**
   * @brief Adds a local waypoint (relative to module).
   */
  void addWaypoint(Vector2 localPos, float tolerance = 1.0f, int id = -1, float angle = 0.0f, bool stop = false);

  /**
   * @brief Returns global waypoints by applying worldPosition to local ones.
   */
  std::vector<Waypoint> getGlobalWaypoints() const;

  // --- Hierarchy ---
  void setParent(Module *p) { parent = p; }
  Module *getParent() const { return parent; }

  // --- Utilities ---
  std::vector<Waypoint> getPath() const;
  const AttachmentPoint *getAttachmentPointByNormal(Vector2 normal) const;

  // --- Spot Management ---
  int getRandomSpotIndex() const;
  Spot getSpot(int index) const;
  void setSpotState(int index, SpotState state);

  struct SpotCounts {
    int free;
    int reserved;
    int occupied;
  };
  SpotCounts getSpotCounts() const;
  float getOccupancyPercentage() const;
  size_t getSpotCount() const { return spots.size(); }

  // --- Type Info ---
  virtual bool isUp() const { return false; }
  const std::vector<Waypoint> &getLocalWaypoints() const { return localWaypoints; }
  virtual ModuleType getType() const { return ModuleType::GENERIC; }

protected:
  float width;
  float height;
  float priceMultiplier = 1.0f;
  std::vector<AttachmentPoint> attachmentPoints;
  std::vector<Waypoint> localWaypoints;
  std::vector<Spot> spots;
  Module *parent = nullptr;
};

// --- Roads ---

class NormalRoad : public Module {
public:
  NormalRoad();
  void draw() const override;
};

class UpEntranceRoad : public Module {
public:
  UpEntranceRoad();
  void draw() const override;
};

class DownEntranceRoad : public Module {
public:
  DownEntranceRoad();
  void draw() const override;
};

class DoubleEntranceRoad : public Module {
public:
  DoubleEntranceRoad();
  void draw() const override;
};

// --- Facilities ---

class SmallParking : public Module {
public:
  SmallParking(bool isTop);
  void draw() const override;
  bool isUp() const override { return isTop; }
  ModuleType getType() const override { return ModuleType::SMALL_PARKING; }

private:
  bool isTop;
};

class LargeParking : public Module {
public:
  LargeParking(bool isTop);
  void draw() const override;
  bool isUp() const override { return isTop; }
  ModuleType getType() const override { return ModuleType::LARGE_PARKING; }

private:
  bool isTop;
};

class SmallChargingStation : public Module {
public:
  SmallChargingStation(bool isTop);
  void draw() const override;
  bool isUp() const override { return isTop; }
  ModuleType getType() const override { return ModuleType::SMALL_CHARGING; }

private:
  bool isTop;
};

class LargeChargingStation : public Module {
public:
  LargeChargingStation(bool isTop);
  void draw() const override;
  bool isUp() const override { return isTop; }
  ModuleType getType() const override { return ModuleType::LARGE_CHARGING; }

private:
  bool isTop;
};
