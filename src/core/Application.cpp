#include "core/Application.hpp"
#include "ui/UIButton.hpp"
#include "core/Logger.hpp"
#include "events/GameEvents.hpp"
#include "events/WindowEvents.hpp"
#include "core/AssetManager.hpp"


/**
 * @file Application.cpp
 * @brief Implementation of the main Application class.
 *
 * Handles the initialization of the game engine, the main run loop,
 * and high-level event management (e.g., window closing).
 */

  Application::Application() {
  Logger::Info("Application Starting...");

  InitAudioDevice();
  backgroundMusic = LoadMusicStream("assets/background_music.mp3");
  if (backgroundMusic.stream.buffer != nullptr) {
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.1f); // Low volume level
    musicLoaded = true;
  }
  
  // Initialize core systems
  eventBus = std::make_shared<EventBus>();
  window = std::make_unique<Window>(eventBus);
  inputSystem = std::make_unique<InputSystem>(eventBus, *window);
  sceneManager = std::make_unique<SceneManager>(eventBus);
  eventLogger = std::make_unique<EventLogger>(eventBus);
  gameLoop = std::make_unique<GameLoop>();

  muteButton = std::make_unique<UIButton>(
        Vector2{ (float)Config::LOGICAL_WIDTH - 75.0f, (float)Config::LOGICAL_HEIGHT - 75.0f },
        Vector2{ 55.0f, 55.0f },
        "",
        eventBus
    );
  muteButton->setOnClick([this]() {
        this->isMuted = !this->isMuted;
    if (this->isMuted) {
        SetMasterVolume(0.0f); 
    } else {
        SetMasterVolume(0.5f); 
    }
    });

  auto &AM = AssetManager::Get();
  AM.LoadTexture("menu_bg", "assets/menu_background.png");
  AM.LoadTexture("config_bg", "assets/config_background.png");

  // تحميل أصول القائمة مسبقاً
  AM.LoadTexture("menu_bg", "assets/menu_background.png");
  AM.LoadTexture("config_bg", "assets/config_background.png");
  
  // أضف هذه الأسطر هنا لضمان ظهورها من البداية
  AM.LoadTexture("sound_on", "assets/sound_on.png");
  AM.LoadTexture("sound_off", "assets/volume-mute.png");

  // Start with the main menu
  sceneManager->setScene(SceneType::MainMenu);

  // Subscribe to the WindowCloseEvent to stop the application loop
  closeEventToken = eventBus->subscribe<WindowCloseEvent>([this](const WindowCloseEvent &) {
    Logger::Info("Window Close Event Received - Stopping Loop");
    isRunning = false;
  });

  // Subscribe to Simulation Speed Changes
  eventTokens.push_back(eventBus->subscribe<SimulationSpeedChangedEvent>(
      [this](const SimulationSpeedChangedEvent &e) { gameLoop->setSpeedMultiplier(e.speedMultiplier); }));

  // Subscribe to Scene Changes to reset speed
  eventTokens.push_back(eventBus->subscribe<SceneChangeEvent>([this](const SceneChangeEvent &e) {
    if (e.newScene != SceneType::Game) {
      gameLoop->setSpeedMultiplier(1.0);
    }
  }));
}

Application::~Application() {
    if (musicLoaded) {
        UnloadMusicStream(backgroundMusic);
    }
    CloseAudioDevice(); 
    Logger::Info("Application Stopped Safely");
}

void Application::run() {
    gameLoop->run(
        [this](double dt) { 
            // Update music stream every frame to ensure it keeps playing
            if (this->musicLoaded) UpdateMusicStream(this->backgroundMusic); 
            this->update(dt); 
        }, 
        [this]() { this->render(); }, 
        [this]() { return isRunning; }
    );
}

void Application::update(double dt) { 
  sceneManager->update(dt);
}

void Application::render() {
  if (window->shouldClose()) {
    eventBus->publish(WindowCloseEvent{});
  }
  inputSystem->update();

  window->beginDrawing();
  sceneManager->render();

 
  if (muteButton) {
      muteButton->draw(); 
      // رسم الأيقونة فوق الزر
      DrawVolumeIcon(
          { (float)Config::LOGICAL_WIDTH - 75.0f, (float)Config::LOGICAL_HEIGHT - 75.0f }, 
          isMuted
      );
    }
    window->endDrawing(); 
}

// src/core/Application.cpp

void Application::DrawVolumeIcon(Vector2 pos, bool muted) {
    auto &AM = AssetManager::Get();
    // اختيار اسم الصورة بناءً على حالة الكتم
    std::string texName = muted ? "sound_off" : "sound_on";
    Texture2D tex = AM.GetTexture(texName);

    if (tex.id > 0) {
        // تحديد حجم الأيقونة (مثلاً 30x30 بكسل) لتناسب الزر الذي حجمه 50x50 أو 55x55
        float iconSize = 32.0f;
        // حساب الإزاحة لتكون الأيقونة في منتصف الزر تماماً
        // الزر في موقع (Config::LOGICAL_WIDTH - 75, Config::LOGICAL_HEIGHT - 75) وحجمه 55
        // الإحداثيات هنا بالنسبة للزر
        float centerX = pos.x + (55.0f - iconSize) / 2.0f;
        float centerY = pos.y + (55.0f - iconSize) / 2.0f;

        Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        Rectangle dest = { centerX, centerY, iconSize, iconSize };
        
        // رسم الصورة باللون الأبيض (للحفاظ على ألوانها الأصلية)
        DrawTexturePro(tex, source, dest, { 0, 0 }, 0.0f, WHITE);
    }
}