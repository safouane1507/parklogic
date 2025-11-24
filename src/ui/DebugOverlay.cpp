#include "ui/DebugOverlay.hpp"
#include "events/GameEvents.hpp"
#include "events/InputEvents.hpp"
#include <format>

DebugOverlay::DebugOverlay(std::shared_ptr<EventBus> bus) : UIElement({10, 10}, {0, 0}, bus) {
  // Default to hidden, toggled by Event
  visible = false;

  toggleEventToken = bus->subscribe<KeyPressedEvent>([this](const KeyPressedEvent &e) {
    if (e.key == KEY_F3) {
      visible = !visible;
      eventBus->publish(ToggleDebugOverlayEvent{visible});
    }
  });
}

void DebugOverlay::update(double /*dt*/) {
  // No specific update logic needed for now
}

void DebugOverlay::draw() {
  if (!visible)
    return;

  int x = static_cast<int>(position.x);
  int y = static_cast<int>(position.y);
  int fontSize = 20;
  int dy = 25;

  struct DebugLine {
    std::string text;
    Color color;
  };
  std::vector<DebugLine> lines;

  lines.push_back(DebugLine{std::format("FPS: {}", GetFPS()), LIME});

  // Calculate dimensions
  int maxWidth = 0;
  for (const auto &line : lines) {
    int width = MeasureText(line.text.c_str(), fontSize);
    if (width > maxWidth)
      maxWidth = width;
  }

  int totalHeight = lines.size() * dy;
  int paddingX = 10;
  int paddingY = 10;

  // Draw Background
  DrawRectangle(x - 5, y - 5, maxWidth + paddingX * 2, totalHeight + paddingY, Fade(BLACK, 0.8f));

  // Draw Text
  for (const auto &line : lines) {
    DrawText(line.text.c_str(), x, y, fontSize, line.color);
    y += dy;
  }
}
