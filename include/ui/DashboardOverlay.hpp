#pragma once
#include "ui/UIElement.hpp"
#include "core/EventBus.hpp"
#include "core/EntityManager.hpp"
#include "events/GameEvents.hpp"
#include <memory>
#include <vector>

class DashboardOverlay : public UIElement {
public:
    DashboardOverlay(std::shared_ptr<EventBus> bus, EntityManager* entityManager);
    ~DashboardOverlay();

    void update(double dt) override;
    void draw() override;

private:
    EntityManager* entityManager;
    std::vector<Subscription> eventTokens; 

    EntitySelectedEvent currentSelection;

    void drawGeneralInfo(int x, int y, int width);
    void drawCarInfo(int x, int y, int width);
    void drawFacilityInfo(int x, int y, int width);
    void drawSpotInfo(int x, int y, int width);

    bool visible = true;
};
