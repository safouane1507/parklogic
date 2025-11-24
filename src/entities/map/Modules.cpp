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

void Module::addWaypoint(Vector2 localPos, float tolerance) { localWaypoints.push_back(Waypoint(localPos, tolerance)); }

std::vector<Waypoint> Module::getGlobalWaypoints() const {
  std::vector<Waypoint> globalWps;
  for (const auto &lwp : localWaypoints) {
    globalWps.emplace_back(Vector2Add(worldPosition, lwp.position), lwp.tolerance, lwp.id);
  }
  return globalWps;
}

// --- Roads ---

NormalRoad::NormalRoad() : Module(10.0f, 7.5f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});

  // Waypoints: Left, Center, Right
  addWaypoint({0, height / 2.0f});
  addWaypoint({width / 2.0f, height / 2.0f});
  addWaypoint({width, height / 2.0f});
}

void NormalRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw(); // Draw outline and waypoints
}

UpEntranceRoad::UpEntranceRoad() : Module(10.0f, 7.5f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
  // Up connection (Top center)
  attachmentPoints.push_back({{width / 2.0f, 0}, {0, -1}});

  // Waypoints
  addWaypoint({0, height / 2.0f});
  addWaypoint({width, height / 2.0f});
  addWaypoint({width / 2.0f, height / 2.0f}); // Center
  addWaypoint({width / 2.0f, 0});             // Up
}

void UpEntranceRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw();

  // Draw entrance indicator
  DrawLineV({worldPosition.x + width / 2.0f, worldPosition.y},
            {worldPosition.x + width / 2.0f, worldPosition.y + height / 2.0f}, WHITE);
}

DownEntranceRoad::DownEntranceRoad() : Module(10.0f, 7.5f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
  // Down connection (Bottom center)
  attachmentPoints.push_back({{width / 2.0f, height}, {0, 1}});

  // Waypoints
  addWaypoint({0, height / 2.0f});
  addWaypoint({width, height / 2.0f});
  addWaypoint({width / 2.0f, height / 2.0f}); // Center
  addWaypoint({width / 2.0f, height});        // Down
}

void DownEntranceRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw();

  // Draw entrance indicator
  DrawLineV({worldPosition.x + width / 2.0f, worldPosition.y + height / 2.0f},
            {worldPosition.x + width / 2.0f, worldPosition.y + height}, WHITE);
}

DoubleEntranceRoad::DoubleEntranceRoad() : Module(10.0f, 7.5f) {
  // Left/Right connections
  attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
  attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
  // Up/Down connections
  attachmentPoints.push_back({{width / 2.0f, 0}, {0, -1}});
  attachmentPoints.push_back({{width / 2.0f, height}, {0, 1}});

  // Waypoints
  addWaypoint({0, height / 2.0f});
  addWaypoint({width, height / 2.0f});
  addWaypoint({width / 2.0f, height / 2.0f}); // Center
  addWaypoint({width / 2.0f, 0});             // Up
  addWaypoint({width / 2.0f, height});        // Down
}

void DoubleEntranceRoad::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, DARKGRAY);
  Module::draw();

  // Draw entrance indicators
  DrawLineV({worldPosition.x + width / 2.0f, worldPosition.y},
            {worldPosition.x + width / 2.0f, worldPosition.y + height}, WHITE);
}

// --- Facilities ---

SmallParking::SmallParking() : Module(10.0f, 7.5f) {
  // Waypoints
  // Assuming attachment at bottom (y=height) or top (y=0)
  // Let's add a central path
  addWaypoint({width / 2.0f, height});        // Bottom entrance
  addWaypoint({width / 2.0f, 0});             // Top entrance
  addWaypoint({width / 2.0f, height / 2.0f}); // Center

  // Parking spot (example)
  addWaypoint({width / 2.0f, height / 2.0f}, 0.5f);
}

void SmallParking::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, LIGHTGRAY);
  Module::draw();

  float fontSize = 0.4f;
  DrawTextEx(GetFontDefault(), "P-Small", {worldPosition.x + 1, worldPosition.y + 1}, fontSize, 0.1f, DARKGRAY);
}

LargeParking::LargeParking() : Module(20.0f, 15.0f) {
  addWaypoint({width / 2.0f, height});
  addWaypoint({width / 2.0f, 0});
  addWaypoint({width / 2.0f, height / 2.0f});
}

void LargeParking::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, LIGHTGRAY);
  Module::draw();

  float fontSize = 0.6f;
  DrawTextEx(GetFontDefault(), "P-Large", {worldPosition.x + 1, worldPosition.y + 1}, fontSize, 0.1f, DARKGRAY);
}

SmallChargingStation::SmallChargingStation() : Module(5.0f, 5.0f) {
  addWaypoint({width / 2.0f, height});
  addWaypoint({width / 2.0f, 0});
  addWaypoint({width / 2.0f, height / 2.0f});
}

void SmallChargingStation::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, GREEN);
  Module::draw();

  float fontSize = 0.3f;
  DrawTextEx(GetFontDefault(), "EV-S", {worldPosition.x + 0.5f, worldPosition.y + 0.5f}, fontSize, 0.1f, WHITE);
}

LargeChargingStation::LargeChargingStation() : Module(10.0f, 10.0f) {
  addWaypoint({width / 2.0f, height});
  addWaypoint({width / 2.0f, 0});
  addWaypoint({width / 2.0f, height / 2.0f});
}

void LargeChargingStation::draw() const {
  DrawRectangleRec({worldPosition.x, worldPosition.y, width, height}, GREEN);
  Module::draw();

  float fontSize = 0.5f;
  DrawTextEx(GetFontDefault(), "EV-L", {worldPosition.x + 1, worldPosition.y + 1}, fontSize, 0.1f, WHITE);
}
