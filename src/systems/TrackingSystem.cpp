#include "systems/TrackingSystem.hpp"
#include "events/GameEvents.hpp"
#include "events/TrackingEvents.hpp"
#include "core/Logger.hpp"

// تنفيذ الـ Constructor (هذا ما يبحث عنه الـ Linker)
TrackingSystem::TrackingSystem(std::shared_ptr<EventBus> bus) : eventBus(bus) {
    // الاشتراك في أحداث بدء وإيقاف التتبع
    eventTokens.push_back(eventBus->subscribe<StartTrackingEvent>([this](const StartTrackingEvent&) {
        this->startTracking();
    }));

    eventTokens.push_back(eventBus->subscribe<StopTrackingEvent>([this](const StopTrackingEvent&) {
        this->stopTracking();
    }));

    // مراقبة السيارات الجديدة
    eventTokens.push_back(eventBus->subscribe<CarSpawnedEvent>([this](const CarSpawnedEvent& e) {
        if (this->waitingForSpawn) {
            this->targetCar = e.car;
            this->waitingForSpawn = false;
            Logger::Info("TrackingSystem: Target car found, start following.");
        }
    }));

    // التحديث الدوري
    eventTokens.push_back(eventBus->subscribe<GameUpdateEvent>([this](const GameUpdateEvent& e) {
        this->update(e.dt);
    }));
}

// تنفيذ الـ Destructor
TrackingSystem::~TrackingSystem() = default;

void TrackingSystem::startTracking() {
    isTrackingActive = true;
    waitingForSpawn = true;
    targetCar = nullptr;
    
    // spawn a car
    eventBus->publish(SpawnCarRequestEvent{});
    eventBus->publish(TrackingStatusEvent{true});
}

void TrackingSystem::stopTracking() {
    isTrackingActive = false;
    targetCar = nullptr;
    waitingForSpawn = false;
    eventBus->publish(TrackingStatusEvent{false});
    Logger::Info("TrackingSystem: Stopped.");
}

void TrackingSystem::update(double) {
    if (!isTrackingActive || !targetCar) return;

    //get car location in meters 
    Vector2 carPos = targetCar->getPosition();

    // send car location to the camera
    eventBus->publish(CameraMoveEvent{carPos});

    // end tracking if the car left
    if (targetCar->getState() == Car::CarState::EXITING && targetCar->hasArrived()) {
        stopTracking();
    }
}