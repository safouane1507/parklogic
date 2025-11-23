#include "entities/map/World.hpp"
#include "config.hpp"
#include "raylib.h"

World::World(float width, float height) : width(width), height(height), showGrid(true) {}

void World::update(double /*dt*/) {
  // World update logic (if any)
}

void World::draw() {
  // Draw World Boundary
  DrawRectangleLines(0, 0, static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

  // Draw Grid
  if (showGrid) {
    // Grid lines every 1 meter
    int spacing = static_cast<int>(Config::PPM);
    int w = static_cast<int>(width * Config::PPM);
    int h = static_cast<int>(height * Config::PPM);

    for (int x = 0; x <= w; x += spacing) {
      DrawLine(x, 0, x, h, Fade(LIGHTGRAY, 0.5f));
    }
    for (int y = 0; y <= h; y += spacing) {
      DrawLine(0, y, w, y, Fade(LIGHTGRAY, 0.5f));
    }
  }
}
