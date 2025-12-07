#include "entities/map/World.hpp"
#include "config.hpp"
#include "core/AssetManager.hpp"
#include "core/Logger.hpp"
#include "raylib.h"
#include <cmath>

World::World(float width, float height) : width(width), height(height), showGrid(true) {
  // Load Textures
  auto &AM = AssetManager::Get();
  AM.LoadTexture("grass1", "assets/grass1.png");
  AM.LoadTexture("grass2", "assets/grass2.png");
  AM.LoadTexture("grass3", "assets/grass3.png");
  AM.LoadTexture("grass4", "assets/grass4.png");

  tileTextures = {"grass1", "grass2", "grass3", "grass4"};

  // Calculate Tile Size in Meters
  // 32 art pixels per tile
  // ART_PIXELS_PER_METER = 7
  float artPixelsPerTile = 32.0f;
  tileWidthMeter = artPixelsPerTile / static_cast<float>(Config::ART_PIXELS_PER_METER);
  tileHeightMeter = tileWidthMeter;

  // Generate Tile Map
  int cols = std::ceil(width / tileWidthMeter);
  int rows = std::ceil(height / tileHeightMeter);

  backgroundTiles.resize(rows, std::vector<int>(cols));

  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      backgroundTiles[y][x] = GetRandomValue(0, tileTextures.size() - 1);
    }
  }

  Logger::Info("World initialized with {}x{} background tiles.", cols, rows);
}

void World::update(double /*dt*/) {
  // World update logic (if any)
}

void World::draw() {
  // Draw Background Tiles
  auto &AM = AssetManager::Get();

  for (size_t y = 0; y < backgroundTiles.size(); ++y) {
    for (size_t x = 0; x < backgroundTiles[y].size(); ++x) {
      int tileIndex = backgroundTiles[y][x];
      Texture2D tex = AM.GetTexture(tileTextures[tileIndex]);
      
      Rectangle source = {0, 0, (float)tex.width, (float)tex.height};
      Rectangle dest = {x * tileWidthMeter, y * tileHeightMeter, tileWidthMeter, tileHeightMeter};
      Vector2 origin = {0, 0};

      DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
    }
  }

  // Draw World Boundary (in Meters)
  DrawRectangleLinesEx({0, 0, width, height}, 0.1f, BLACK);

  // Draw Grid
  if (showGrid) {
    // Grid lines every 1 meter
    float spacing = 1.0f;

    for (float x = 0; x <= width; x += spacing) {
      DrawLineV({x, 0}, {x, height}, Fade(LIGHTGRAY, 0.3f));
    }
    for (float y = 0; y <= height; y += spacing) {
      DrawLineV({0, y}, {width, y}, Fade(LIGHTGRAY, 0.3f));
    }
  }
}
