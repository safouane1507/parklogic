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
    SetMusicVolume(backgroundMusic, 0.1f); // نصف مستوى الصوت
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
        "ON",
        eventBus
    );
  muteButton->setOnClick([this]() {
        this->isMuted = !this->isMuted;
        if (this->isMuted) {
            SetMasterVolume(0.0f);
            this->muteButton->setText("OFF");
        } else {
            SetMasterVolume(0.5f);
            this->muteButton->setText("ON");
        }
    });

  auto &AM = AssetManager::Get();
  AM.LoadTexture("menu_bg", "assets/menu_background.png");
  AM.LoadTexture("config_bg", "assets/config_background.png");

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
            // تحديث الموسيقى في كل فريم لضمان استمرارها
            if (this->musicLoaded) UpdateMusicStream(this->backgroundMusic); 
            this->update(dt); 
        }, 
        [this]() { this->render(); }, 
        [this]() { return isRunning; }
    );
}

void Application::update(double dt) { sceneManager->update(dt); }

void Application::render() {
  if (window->shouldClose()) {
    eventBus->publish(WindowCloseEvent{});
  }
  inputSystem->update();

  window->beginDrawing();
  sceneManager->render();

 if (muteButton) muteButton->draw();

  window->endDrawing();
}
