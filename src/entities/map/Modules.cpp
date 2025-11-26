#include "entities/map/Modules.hpp"
#include "raylib.h"
#include "raymath.h"

// --- Module Base Class ---

void Module::draw() const {
  // Default draw: outline (in Meters)
  DrawRectangleLinesEx({worldPosition.x, worldPosition.y, width, height}, 0.1f, BLACK);

  // Draw Waypoints (Debug)
  for (const auto &lwp : localWaypoints) {
    Vector2 globalPos = Vector2Add(worldPosition, lwp.position);
    DrawCircleV(globalPos, 0.2f, Fade(ORANGE, 0.6f));
  }
}

void Module::addWaypoint(Vector2 localPos, float tolerance, int id, float angle, bool stop) {
    localWaypoints.emplace_back(localPos, tolerance, id, angle, stop);
}

std::vector<Waypoint> Module::getGlobalWaypoints() const {
  std::vector<Waypoint> globalWps;
  for (const auto &lwp : localWaypoints) {
    globalWps.emplace_back(Vector2Add(worldPosition, lwp.position), lwp.tolerance, lwp.id, lwp.entryAngle, lwp.stopAtEnd);
  }
  return globalWps;
}

std::vector<Waypoint> Module::getPath() const {
    std::vector<Waypoint> path;
    
    // 1. Get Parent's path first (Recursive)
    if (parent) {
        path = parent->getPath();
    }

    // 2. Append my own waypoints
    // Note: We might want to filter or select specific waypoints, but for now, append all.
    // Usually a module like a Road has 1 center waypoint.
    // A facility has 1 center waypoint.
    std::vector<Waypoint> myWps = getGlobalWaypoints();
    path.insert(path.end(), myWps.begin(), myWps.end());

    return path;
}

// --- Roads ---

NormalRoad::NormalRoad() : Module(20.0f, 15.0f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});

  // Waypoints: Center only (Root of path)
  addWaypoint({width / 2.0f, height / 2.0f});
}

void NormalRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw(); // Draw outline and waypoints
}

UpEntranceRoad::UpEntranceRoad() : Module(20.0f, 15.0f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
  // Up connection (Top center)
  attachmentPoints.push_back({{width / 2.0f, 0}, {0, -1}});

  // Waypoints: Center only
  addWaypoint({width / 2.0f, height / 2.0f}); // Center
}

void UpEntranceRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw();

  // Draw entrance indicator
  DrawLineV({worldPosition.x + width / 2.0f, worldPosition.y},
            {worldPosition.x + width / 2.0f, worldPosition.y + height / 2.0f}, WHITE);
}

DownEntranceRoad::DownEntranceRoad() : Module(20.0f, 15.0f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
  // Down connection (Bottom center)
  attachmentPoints.push_back({{width / 2.0f, height}, {0, 1}});

  // Waypoints: Center only
  addWaypoint({width / 2.0f, height / 2.0f}); // Center
}

void DownEntranceRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw();

  // Draw entrance indicator
  DrawLineV({worldPosition.x + width / 2.0f, worldPosition.y + height / 2.0f},
            {worldPosition.x + width / 2.0f, worldPosition.y + height}, WHITE);
}

DoubleEntranceRoad::DoubleEntranceRoad() : Module(20.0f, 15.0f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
  // Up/Down connections
  attachmentPoints.push_back({{width / 2.0f, 0}, {0, -1}});
  attachmentPoints.push_back({{width / 2.0f, height}, {0, 1}});

  // Waypoints: Center only
  addWaypoint({width / 2.0f, height / 2.0f}); // Center
}

void DoubleEntranceRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw();

  // Draw entrance indicators
  DrawLineV({worldPosition.x + width / 2.0f, worldPosition.y},
            {worldPosition.x + width / 2.0f, worldPosition.y + height}, WHITE);
}

// --- Facilities ---

SmallParking::SmallParking() : Module(30.0f, 25.0f) {
  // Waypoints: Center only
  addWaypoint({width / 2.0f, height / 2.0f}); // Center
}

void SmallParking::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, LIGHTGRAY);
  Module::draw();

  float fontSize = 0.4f;
  DrawTextEx(GetFontDefault(), "P-Small", {worldPosition.x + 1, worldPosition.y + 1}, fontSize, 0.1f, DARKGRAY);
}

LargeParking::LargeParking() : Module(60.0f, 50.0f) {
  addWaypoint({width / 2.0f, height / 2.0f});
}

void LargeParking::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, LIGHTGRAY);
  Module::draw();

  float fontSize = 0.6f;
  DrawTextEx(GetFontDefault(), "P-Large", {worldPosition.x + 1, worldPosition.y + 1}, fontSize, 0.1f, DARKGRAY);
}

SmallChargingStation::SmallChargingStation() : Module(15.0f, 15.0f) {
  addWaypoint({width / 2.0f, height / 2.0f});
}

void SmallChargingStation::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, GREEN);
  Module::draw();

  float fontSize = 0.3f;
  DrawTextEx(GetFontDefault(), "EV-S", {worldPosition.x + 0.5f, worldPosition.y + 0.5f}, fontSize, 0.1f, WHITE);
}

LargeChargingStation::LargeChargingStation() : Module(30.0f, 30.0f) {
  addWaypoint({width / 2.0f, height / 2.0f});
}

void LargeChargingStation::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, GREEN);
  Module::draw();

  float fontSize = 0.5f;
  DrawTextEx(GetFontDefault(), "EV-L", {worldPosition.x + 1, worldPosition.y + 1}, fontSize, 0.1f, WHITE);
}
