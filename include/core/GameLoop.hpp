#pragma once
#include <functional>
/**
 * @class GameLoop
 * @brief Manages the main game loop with a fixed timestep.
 *
 * The GameLoop class implements a fixed timestep game loop, ensuring consistent
 * game logic updates regardless of the rendering framerate.
 */
class GameLoop {
public:
  /**
   * @brief Runs the game loop.
   *
   * @param update Function to call for updating game logic (receives delta time).
   * @param render Function to call for rendering the frame.
   * @param running Function that returns true if the loop should continue.
   */
  void run(std::function<void(double)> update, std::function<void()> render, std::function<bool()> running);
  
  /**
    * @brief Sets the speed multiplier for the simulation.
    * @param speed The speed multiplier (e.g., 1.0 for normal speed, 2.0 for double speed).
    */
  void setSpeedMultiplier(double speed) { speedMultiplier = speed; }

private:
  double speedMultiplier = 1.0;
};
