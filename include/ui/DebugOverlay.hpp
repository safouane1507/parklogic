#pragma once
#include "ui/UIElement.hpp"

/**
 * @class DebugOverlay
 * @brief Displays debug information on the screen.
 *
 * Shows FPS.
 */
class DebugOverlay : public UIElement {
public:
  /**
   * @brief Constructs the DebugOverlay.
   *
   * @param bus Shared pointer to the EventBus.
   */
  DebugOverlay(std::shared_ptr<EventBus> bus);

  void update(double dt) override;
  void draw() override;

private:
  Subscription toggleEventToken;
};
