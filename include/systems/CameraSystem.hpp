#pragma once
#include "core/EventBus.hpp"
#include "raylib.h"
#include <memory>
#include <set>
#include <vector>

/**
 * @class CameraSystem
 * @brief Manages the 2D game camera.
 *
 * Handles:
 * - Zooming (Mouse Wheel) with Multiplicative scaling.
 * - Panning (WASD Keys).
 * - Clamping to World Bounds.
 * - Coordinate transformation (World <-> Screen).
 */
class CameraSystem {
public:
  /**
   * @brief Constructs the CameraSystem.
   * @param bus EventBus for receiving I/O and Game events.
   */
  explicit CameraSystem(std::shared_ptr<EventBus> bus);
  ~CameraSystem();

  /**
   * @brief Updates camera position based on input and physics.
   * @param dt Delta time.
   */
  void update(double dt);

  /**
   * @brief Sets the world limits to prevent the camera from straying too far.
   * @param width World width in Meters.
   * @param height World height in Meters.
   */
  void setWorldBounds(float width, float height);

  /**
   * @brief Gets the underlying Raylib camera object.
   * @return Camera2D struct.
   */
  Camera2D getCamera() const { return camera; }

  // Setters for initial setup
  void setTarget(Vector2 target) { camera.target = target; }
  void setOffset(Vector2 offset) { camera.offset = offset; }
  void setZoom(float zoom) { camera.zoom = zoom; }

private:
  std::shared_ptr<EventBus> eventBus;
  std::vector<Subscription> eventTokens;

  Camera2D camera = {{0, 0}, {0, 0}, 0.0f, 1.0f};

  float worldWidth = 0.0f;
  float worldHeight = 0.0f;
  bool boundsSet = false;

  std::set<int> keysDown;
  double speedMultiplier = 1.0;
};
