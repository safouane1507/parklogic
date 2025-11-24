#pragma once
#include "raylib.h"

struct Waypoint {
    Vector2 position; // Global position in meters
    float tolerance;  // Radius in meters to consider "reached"
    int id;           // Optional ID for identification

    Waypoint(Vector2 pos, float tol = 1.0f, int _id = -1) 
        : position(pos), tolerance(tol), id(_id) {}
};
