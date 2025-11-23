#pragma once
#include "entities/Entity.hpp"

/**
 * @class World
 * @brief Represents the game world boundaries and grid.
 *
 * The World class manages the playable area, rendering the boundary lines and
 * an optional grid for visual reference.
 */

class World : public Entity {
public:
  World(float width, float height);

  void update(double dt) override;
  void draw() override;

  void setGridEnabled(bool enabled) { showGrid = enabled; }
  bool isGridEnabled() const { return showGrid; }
  void toggleGrid() { showGrid = !showGrid; }

  float getWidth() const { return width; }
  float getHeight() const { return height; }

private:
  float width;
  float height;
  bool showGrid;
};
