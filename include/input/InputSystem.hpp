#pragma once

/**
 * @file InputSystem.hpp
 * @brief Input handling system.
 */
#include "core/EventBus.hpp"
#include "core/Window.hpp"
#include <unordered_set>

/**
 * @class InputSystem
 * @brief Handles low-level input polling (Keyboard/Mouse) and event dispatching.
 *
 * It acts as the bridge between Raylib's input polling (IsKeyPressed) and the EventBus.
 * It also translates screen coordinates to logical game coordinates.
 */
class InputSystem {
public:
  /**
   * @brief Constructs the InputSystem.
   * @param bus EventBus for publishing input events.
   * @param win Window reference for coordinate translation.
   */
  InputSystem(std::shared_ptr<EventBus> bus, const Window &win);

  /**
   * @brief Polls input and dispatches events.
   *
   * Should be called once per frame (not in fixed update).
   */
  void update();

private:
  std::shared_ptr<EventBus> eventBus;
  const Window &window;
  std::unordered_set<int> activeKeys; ///< Tracks currently held keys for KeyReleased events.
};
