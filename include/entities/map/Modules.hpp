#pragma once
#include "config.hpp"
#include "entities/map/Module.hpp"

// --- Roads ---

class NormalRoad : public Module {
public:
  NormalRoad() : Module(10.0f, 7.5f) {
    // Left/Right connections
    attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
    attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
  }

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), DARKGRAY);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);
  }
};

class UpEntranceRoad : public Module {
public:
  UpEntranceRoad() : Module(10.0f, 7.5f) {
    // Left/Right connections
    attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
    attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
    // Up connection (Top center)
    attachmentPoints.push_back({{width / 2.0f, 0}, {0, -1}});
  }

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), DARKGRAY);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

    // Draw entrance indicator
    DrawLine(static_cast<int>((worldPosition.x + width / 2.0f) * Config::PPM),
             static_cast<int>(worldPosition.y * Config::PPM),
             static_cast<int>((worldPosition.x + width / 2.0f) * Config::PPM),
             static_cast<int>((worldPosition.y + height / 2.0f) * Config::PPM), WHITE);
  }
};

class DownEntranceRoad : public Module {
public:
  DownEntranceRoad() : Module(10.0f, 7.5f) {
    // Left/Right connections
    attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
    attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
    // Down connection (Bottom center)
    attachmentPoints.push_back({{width / 2.0f, height}, {0, 1}});
  }

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), DARKGRAY);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

    // Draw entrance indicator
    DrawLine(static_cast<int>((worldPosition.x + width / 2.0f) * Config::PPM),
             static_cast<int>((worldPosition.y + height / 2.0f) * Config::PPM),
             static_cast<int>((worldPosition.x + width / 2.0f) * Config::PPM),
             static_cast<int>((worldPosition.y + height) * Config::PPM), WHITE);
  }
};

class DoubleEntranceRoad : public Module {
public:
  DoubleEntranceRoad() : Module(10.0f, 7.5f) {
    // Left/Right connections
    attachmentPoints.push_back({{0, height / 2.0f}, {-1, 0}});
    attachmentPoints.push_back({{width, height / 2.0f}, {1, 0}});
    // Up/Down connections
    attachmentPoints.push_back({{width / 2.0f, 0}, {0, -1}});
    attachmentPoints.push_back({{width / 2.0f, height}, {0, 1}});
  }

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), DARKGRAY);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

    // Draw entrance indicators
    DrawLine(static_cast<int>((worldPosition.x + width / 2.0f) * Config::PPM),
             static_cast<int>(worldPosition.y * Config::PPM),
             static_cast<int>((worldPosition.x + width / 2.0f) * Config::PPM),
             static_cast<int>((worldPosition.y + height) * Config::PPM), WHITE);
  }
};

// --- Facilities ---

class SmallParking : public Module {
public:
  SmallParking() : Module(10.0f, 7.5f) {
    // Attachment at bottom (for top placement) and top (for bottom placement)
    // We'll just define where it *can* attach.
    // Usually facilities attach to the road.
    // Let's assume we align them based on the road's attachment point.
  }

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), LIGHTGRAY);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

    int fontSize = static_cast<int>(0.4f * Config::PPM);
    DrawText("P-Small", static_cast<int>((worldPosition.x + 1) * Config::PPM),
             static_cast<int>((worldPosition.y + 1) * Config::PPM), fontSize, DARKGRAY);
  }
};

class LargeParking : public Module {
public:
  LargeParking() : Module(20.0f, 15.0f) {}

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), LIGHTGRAY);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

    int fontSize = static_cast<int>(0.6f * Config::PPM);
    DrawText("P-Large", static_cast<int>((worldPosition.x + 1) * Config::PPM),
             static_cast<int>((worldPosition.y + 1) * Config::PPM), fontSize, DARKGRAY);
  }
};

class SmallChargingStation : public Module {
public:
  SmallChargingStation() : Module(5.0f, 5.0f) {}

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), GREEN);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

    int fontSize = static_cast<int>(0.3f * Config::PPM);
    DrawText("EV-S", static_cast<int>((worldPosition.x + 0.5f) * Config::PPM),
             static_cast<int>((worldPosition.y + 0.5f) * Config::PPM), fontSize, WHITE);
  }
};

class LargeChargingStation : public Module {
public:
  LargeChargingStation() : Module(10.0f, 10.0f) {}

  void draw() const override {
    DrawRectangle(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                  static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), GREEN);
    DrawRectangleLines(static_cast<int>(worldPosition.x * Config::PPM), static_cast<int>(worldPosition.y * Config::PPM),
                       static_cast<int>(width * Config::PPM), static_cast<int>(height * Config::PPM), BLACK);

    int fontSize = static_cast<int>(0.5f * Config::PPM);
    DrawText("EV-L", static_cast<int>((worldPosition.x + 1) * Config::PPM),
             static_cast<int>((worldPosition.y + 1) * Config::PPM), fontSize, WHITE);
  }
};
