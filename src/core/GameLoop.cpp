#include "core/GameLoop.hpp"
#include "config.hpp"
#include "raylib.h"

/**
 * @file GameLoop.cpp
 * @brief Implementation of the fixed-timestep game loop.
 */

/**
 * @brief Runs the game loop.
 *
 * Implementation of the "Fix Your Timestep" pattern.
 * - Accumulates elapsed time in a buffer.
 * - Consumes time in fixed slices (dt) for logic updates (Physics, AI).
 * - Renders once per frame using the remaining state.
 */
void GameLoop::run(std::function<void(double)> update, std::function<void()> render, std::function<bool()> running) {

  const double dt = Config::FIXED_DELTA_TIME;
  double currentTime = GetTime();
  double accumulator = 0.0;

  while (running()) {
    double newTime = GetTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;

    // Cap frame time to avoid spiral of death
    if (frameTime > 0.25)
      frameTime = 0.25;

    // Apply speed multiplier to accumulating time
    frameTime *= speedMultiplier;

    accumulator += frameTime;

    // Fixed timestep update
    while (accumulator >= dt) {
      update(dt);
      accumulator -= dt;
    }
    render();
  }
}
