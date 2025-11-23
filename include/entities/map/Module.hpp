#pragma once
#include "raylib.h"
#include <vector>

struct AttachmentPoint {
  Vector2 position; // Relative to module top-left
  Vector2 normal;   // Direction of attachment
};

#include "config.hpp"

class Module {
public:
  Module(float w, float h) : width(w), height(h) {}
  virtual ~Module() = default;

  float getWidth() const { return width; }
  float getHeight() const { return height; }

  const std::vector<AttachmentPoint> &getAttachmentPoints() const { return attachmentPoints; }

  // Position in the world (set during generation)
  Vector2 worldPosition = {0, 0};

  virtual void draw() const {
    // Default draw: outline
    // Scale by PPM
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);
  }

protected:
  float width;
  float height;
  std::vector<AttachmentPoint> attachmentPoints;
};
