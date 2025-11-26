#pragma once
#include <vector>
#include "entities/map/Waypoint.hpp"

struct MapConfig {
  int smallParkingCount = 2;
  int largeParkingCount = 2;
  int smallChargingCount = 2;
  int largeChargingCount = 2;
};

enum class SceneType { MainMenu, MapConfig, Game };
struct SceneChangeEvent {
  SceneType newScene;
  MapConfig config;
};

struct GamePausedEvent {};
struct GameResumedEvent {};
struct ToggleDebugOverlayEvent {
  bool enabled;
};

struct CameraZoomEvent {
  float zoomDelta;
};

struct SpawnCarEvent {};

struct CarSpawnedEvent {
    class Car* car;
};

struct AssignPathEvent {
    class Car* car;
    std::vector<struct Waypoint> path;
};
