#include "ui/UIButton.hpp"
#include "events/InputEvents.hpp"
#include "raylib.h"

static Sound clickSound;
static bool clickSoundLoaded = false;

/**
 * @file UIButton.cpp
 * @brief Implementation of a clickable UI button.
 */

UIButton::UIButton(Vector2 pos, Vector2 sz, const std::string &buttonText, std::shared_ptr<EventBus> bus)
    : UIElement(pos, sz, bus), text(buttonText) {

  if (!clickSoundLoaded) {
    // Load in mp3 format
    clickSound = LoadSound("assets/click_sound.mp3");

    if (clickSound.frameCount > 0) {
      SetSoundVolume(clickSound, 1.0f);
      clickSoundLoaded = true;
      TraceLog(LOG_INFO, "Click sound (mp3) loaded successfully!");
    } else {
      // Attempt to load from an alternative path if the first fails
      clickSound = LoadSound("../assets/click_sound.mp3");
      if (clickSound.frameCount > 0)
        clickSoundLoaded = true;
    }
  }

  // 1. Subscribe to Mouse Move (Keep token!)
  // Updates hover state based on mouse position
  tokens.push_back(eventBus->subscribe<MouseMovedEvent>([this](const MouseMovedEvent &e) {
    if (!visible)
      return;
    Rectangle rect = {position.x, position.y, size.x, size.y};
    isHovered = CheckCollisionPointRec(e.position, rect);
  }));

  // 2. Subscribe to Mouse Click (Keep token!)
  // Handles click logic: press down, release inside to trigger
  tokens.push_back(eventBus->subscribe<MouseClickEvent>([this](const MouseClickEvent &e) {
    if (!visible)
      return;

    // Only interact if hovered
    if (isHovered && e.button == MOUSE_BUTTON_LEFT) {
      if (e.down) {
        isPressed = true;
      } else if (isPressed) {
        // --- Major logic update here ---
        if (clickSoundLoaded) {
          SetSoundVolume(clickSound, 0.3f); // Ensure the sound is at maximum level
          StopSound(clickSound);            // Stop any previous button sound (to avoid overlap)
          PlaySound(clickSound);            // Play sound immediately
        }

        if (onClick)
          onClick();
        isPressed = false;
      }
    } else if (!e.down) {
      isPressed = false;
    }
  }));
}

void UIButton::draw() {
  if (!visible)
    return;

  // 1. Define neon colors (blue and purple)
  Color baseColor = {30, 30, 70, 180};   // Transparent dark blue for the background
  Color hoverColor = {140, 0, 255, 230}; // Neon purple on mouse hover
  Color pressColor = {0, 250, 255, 255}; // Neon sky blue on click

  // Select color based on button state
  Color currentC = isPressed ? pressColor : (isHovered ? hoverColor : baseColor);

  // 2. Draw button body (using provided position and size)
  DrawRectangleV(position, size, currentC);

  // 3. Draw glowing neon border
  float lineThickness = isHovered ? 3.0f : 1.5f;
  // Light blue "Cyan" border on hover, and faded purple in normal state
  Color borderColor = isHovered ? Color{0, 255, 255, 255} : Color{100, 100, 200, 255};

  Rectangle btnRect = {position.x, position.y, size.x, size.y};
  DrawRectangleLinesEx(btnRect, lineThickness, borderColor);

  // 4. Draw text in white for clarity
  int fSize = 22;
  int txtW = MeasureText(this->text.c_str(), fSize);

  DrawText(this->text.c_str(), (int)(position.x + (size.x - txtW) / 2), (int)(position.y + (size.y - fSize) / 2), fSize,
           WHITE);
}
void UIButton::setText(const std::string &t) { this->text = t; }
void UIButton::setOnClick(std::function<void()> cb) { onClick = cb; }
void UIButton::update(double) {}
