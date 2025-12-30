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
    // تحميل بصيغة mp3
    clickSound = LoadSound("assets/click_sound.mp3"); 
    
    if (clickSound.frameCount > 0) {
        SetSoundVolume(clickSound, 1.0f);
        clickSoundLoaded = true;
        TraceLog(LOG_INFO, "Click sound (mp3) loaded successfully!");
    } else {
        // محاولة التحميل بمسار بديل إذا فشل الأول
        clickSound = LoadSound("../assets/click_sound.mp3");
        if (clickSound.frameCount > 0) clickSoundLoaded = true;
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
        // --- التعديل القوي هنا ---
        if (clickSoundLoaded) {
            SetSoundVolume(clickSound, 1.0f); // التأكد من أن الصوت في أقصى مستوى
            StopSound(clickSound);            // إيقاف أي صوت سابق للزر (لتجنب التداخل)
            PlaySound(clickSound);           // تشغيل الصوت فوراً
        }
        
        if (onClick) onClick();
        isPressed = false;
      }
    }else if (!e.down) {
            isPressed = false;
        }
    }));  
  }

    void UIButton::draw() {
    if (!visible) return;

    
    // 1. تعريف ألوان النيون (أزرق وبنفسجي)
    Color baseColor   = { 30, 30, 70, 180 };    // أزرق داكن شفاف للخلفية
    Color hoverColor  = { 140, 0, 255, 230 };   // بنفسجي نيون عند مرور الماوس
    Color pressColor  = { 0, 250, 255, 255 };   // أزرق سماوي نيون عند الضغط

    // اختيار اللون بناءً على حالة الزر
    Color currentC = isPressed ? pressColor : (isHovered ? hoverColor : baseColor);

    // 2. رسم جسم الزر (باستخدام position و size من الكود الخاص بك)
    DrawRectangleV(position, size, currentC);
    
    // 3. رسم إطار النيون المتوهج
    float lineThickness = isHovered ? 3.0f : 1.5f;
    // إطار أزرق فاتح "Cyan" عند التمرير، وبنفسجي باهت في الحالة العادية
    Color borderColor = isHovered ? Color{ 0, 255, 255, 255 } : Color{ 100, 100, 200, 255 };
    
    Rectangle btnRect = { position.x, position.y, size.x, size.y };
    DrawRectangleLinesEx(btnRect, lineThickness, borderColor);

    // 4. رسم النص باللون الأبيض ليكون واضحاً
    int fSize = 22; 
    int txtW = MeasureText(this->text.c_str(), fSize); 
    
    DrawText(this->text.c_str(), 
             (int)(position.x + (size.x - txtW) / 2), 
             (int)(position.y + (size.y - fSize) / 2), 
             fSize, WHITE);
}
void UIButton::setText(const std::string &t) { this->text = t; }
void UIButton::setOnClick(std::function<void()> cb) { onClick = cb; }
void UIButton::update(double) {}