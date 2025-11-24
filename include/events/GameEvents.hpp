#pragma once

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
