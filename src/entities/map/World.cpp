#include "entities/map/World.hpp"
#include "raylib.h"

World::World(float width, float height) : width(width), height(height), showGrid(true) {}

void World::update(double /*dt*/) {
  // World update logic (if any)
}

void World::draw() {
  // Draw World Boundary (in Meters)
  // Use DrawRectangleLinesEx for float thickness (e.g., 0.1m)
  DrawRectangleLinesEx({0, 0, width, height}, 0.1f, BLACK);

  // Draw Grid
  if (showGrid) {
    // Grid lines every 1 meter
    float spacing = 1.0f;

    for (float x = 0; x <= width; x += spacing) {
      DrawLineV({x, 0}, {x, height}, Fade(LIGHTGRAY, 0.5f));
    }
    for (float y = 0; y <= height; y += spacing) {
      DrawLineV({0, y}, {width, y}, Fade(LIGHTGRAY, 0.5f));
    }
  }
}
