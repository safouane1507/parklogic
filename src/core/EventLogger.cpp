#include "core/EventLogger.hpp"
#include "core/Logger.hpp"
#include "events/GameEvents.hpp"
#include "events/InputEvents.hpp"
#include "events/WindowEvents.hpp"

EventLogger::EventLogger(std::shared_ptr<EventBus> bus) : eventBus(bus) {
  // Subscribe to various events and log them
  subscriptions.push_back(eventBus->subscribe<SceneChangeEvent>(
      [](const SceneChangeEvent &e) { Logger::Info("Event: SceneChangeEvent [NewScene: {}]", (int)e.newScene); }));

  subscriptions.push_back(eventBus->subscribe<KeyPressedEvent>(
      [](const KeyPressedEvent &e) { Logger::Info("Event: KeyPressedEvent [Key: {}]", e.key); }));

  subscriptions.push_back(eventBus->subscribe<KeyReleasedEvent>(
      [](const KeyReleasedEvent &e) { Logger::Info("Event: KeyReleasedEvent [Key: {}]", e.key); }));

  subscriptions.push_back(eventBus->subscribe<MouseMovedEvent>([](const MouseMovedEvent & /*e*/) {
    // Commented out to avoid spamming logs, uncomment if needed
    // Logger::Info("Event: MouseMovedEvent [x: {}, y: {}]", e.position.x, e.position.y);
  }));

  subscriptions.push_back(
      eventBus->subscribe<GamePausedEvent>([](const GamePausedEvent &) { Logger::Info("Event: GamePausedEvent"); }));

  subscriptions.push_back(
      eventBus->subscribe<GameResumedEvent>([](const GameResumedEvent &) { Logger::Info("Event: GameResumedEvent"); }));

  subscriptions.push_back(eventBus->subscribe<MouseClickEvent>([](const MouseClickEvent &e) {
    Logger::Info("Event: MouseClickEvent [Button: {}, x: {}, y: {}, Down: {}]", e.button, e.position.x, e.position.y,
                 e.down);
  }));

  subscriptions.push_back(eventBus->subscribe<WindowResizeEvent>([](const WindowResizeEvent &e) {
    Logger::Info("Event: WindowResizeEvent [Width: {}, Height: {}]", e.width, e.height);
  }));

  subscriptions.push_back(
      eventBus->subscribe<WindowCloseEvent>([](const WindowCloseEvent &) { Logger::Info("Event: WindowCloseEvent"); }));

  subscriptions.push_back(eventBus->subscribe<CameraZoomEvent>(
      [](const CameraZoomEvent &e) { Logger::Info("Event: CameraZoomEvent [Delta: {}]", e.zoomDelta); }));

  subscriptions.push_back(eventBus->subscribe<GenerateWorldEvent>(
      [](const GenerateWorldEvent &) { Logger::Info("Event: GenerateWorldEvent"); }));

  subscriptions.push_back(eventBus->subscribe<WorldBoundsEvent>(
      [](const WorldBoundsEvent &e) { Logger::Info("Event: WorldBoundsEvent [W: {}, H: {}]", e.width, e.height); }));

  subscriptions.push_back(eventBus->subscribe<ToggleDashboardEvent>(
      [](const ToggleDashboardEvent &) { Logger::Info("Event: ToggleDashboardEvent"); }));

  subscriptions.push_back(
      eventBus->subscribe<SpawnCarEvent>([](const SpawnCarEvent &) { Logger::Info("Event: SpawnCarEvent"); }));

  subscriptions.push_back(eventBus->subscribe<CycleAutoSpawnLevelEvent>(
      [](const CycleAutoSpawnLevelEvent &) { Logger::Info("Event: CycleAutoSpawnLevelEvent"); }));

  subscriptions.push_back(eventBus->subscribe<AutoSpawnLevelChangedEvent>([](const AutoSpawnLevelChangedEvent &e) {
    Logger::Info("Event: AutoSpawnLevelChangedEvent [Level: {}]", e.newLevel);
  }));

  subscriptions.push_back(eventBus->subscribe<SpawnCarRequestEvent>(
      [](const SpawnCarRequestEvent &) { Logger::Info("Event: SpawnCarRequestEvent"); }));

  subscriptions.push_back(eventBus->subscribe<CreateCarEvent>(
      [](const CreateCarEvent &e) { Logger::Info("Event: CreateCarEvent [Type: {}]", e.carType); }));

  subscriptions.push_back(
      eventBus->subscribe<CarSpawnedEvent>([](const CarSpawnedEvent &) { Logger::Info("Event: CarSpawnedEvent"); }));

  subscriptions.push_back(eventBus->subscribe<AssignPathEvent>(
      [](const AssignPathEvent &e) { Logger::Info("Event: AssignPathEvent [PathSize: {}]", e.path.size()); }));

  subscriptions.push_back(eventBus->subscribe<CarFinishedParkingEvent>(
      [](const CarFinishedParkingEvent &) { Logger::Info("Event: CarFinishedParkingEvent"); }));

  subscriptions.push_back(
      eventBus->subscribe<CarDespawnEvent>([](const CarDespawnEvent &) { Logger::Info("Event: CarDespawnEvent"); }));

  subscriptions.push_back(eventBus->subscribe<SimulationSpeedChangedEvent>([](const SimulationSpeedChangedEvent &e) {
    Logger::Info("Event: SimulationSpeedChangedEvent [Mul: {}]", e.speedMultiplier);
  }));

  subscriptions.push_back(eventBus->subscribe<EntitySelectedEvent>(
      [](const EntitySelectedEvent &e) { Logger::Info("Event: EntitySelectedEvent [Type: {}]", (int)e.type); }));
}
