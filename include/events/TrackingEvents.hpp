#pragma once

struct StartTrackingEvent {};
struct StopTrackingEvent {};

struct TrackingStatusEvent {
    bool isTracking;
};