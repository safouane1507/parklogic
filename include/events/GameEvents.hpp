#pragma once
#include <vector>
#include "entities/map/Waypoint.hpp"

enum class SceneType { MainMenu, Game };
struct SceneChangeEvent {
  SceneType newScene;
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
