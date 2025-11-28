#include "systems/TrafficSystem.hpp"
#include "events/GameEvents.hpp"
#include "core/Logger.hpp"
#include "entities/map/Modules.hpp"

TrafficSystem::TrafficSystem(std::shared_ptr<EventBus> bus, const EntityManager& em) 
    : eventBus(bus), entityManager(em) {
    
    // 1. Handle Spawn Request -> Find Position -> Publish CreateCarEvent
    eventTokens.push_back(eventBus->subscribe<SpawnCarRequestEvent>([this](const SpawnCarRequestEvent&) {
        Logger::Info("TrafficSystem: Processing Spawn Request...");
        
        // Find a valid start position (Road)
        Vector2 spawnPos = {0, 0};
        bool found = false;
        
        const auto& modules = entityManager.getModules();
        for (const auto &mod : modules) {
            if (auto *r = dynamic_cast<NormalRoad *>(mod.get())) {
                std::vector<Waypoint> wps = r->getGlobalWaypoints();
                if (!wps.empty())
                    spawnPos = wps[0].position;
                else
                    spawnPos = r->worldPosition;
                found = true;
                break;
            }
        }
        
        if (found) {
            eventBus->publish(CreateCarEvent{spawnPos});
        } else {
            Logger::Error("TrafficSystem: No valid spawn point found.");
        }
    }));

    // 2. Handle Car Spawned -> Calculate Path -> Publish AssignPathEvent
    eventTokens.push_back(eventBus->subscribe<CarSpawnedEvent>([this](const CarSpawnedEvent& e) {
        Logger::Info("TrafficSystem: Calculating path for new car...");
        
        std::vector<Module *> facilities;
        const auto& modules = entityManager.getModules();
        for (const auto &mod : modules) {
            if (dynamic_cast<SmallParking *>(mod.get()) || dynamic_cast<LargeParking *>(mod.get()) ||
                dynamic_cast<SmallChargingStation *>(mod.get()) || dynamic_cast<LargeChargingStation *>(mod.get())) {
                facilities.push_back(mod.get());
            }
        }

        if (facilities.empty()) {
            Logger::Error("TrafficSystem: No facilities found.");
            return;
        }

        // Pick random facility
        int idx = GetRandomValue(0, (int)facilities.size() - 1);
        Module *targetFac = facilities[idx];

        // Get Path
        std::vector<Waypoint> path = targetFac->getPath();

        // Publish Path Assignment
        eventBus->publish(AssignPathEvent{e.car, path});
    }));
}

TrafficSystem::~TrafficSystem() {
    eventTokens.clear();
}
