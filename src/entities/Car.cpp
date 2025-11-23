#include "entities/Car.hpp"
#include "config.hpp"
#include "entities/map/World.hpp"
#include "raymath.h"
#include <memory>
#include <vector>

/**
 * @brief Constructs a new Car object.
 *
 * @param startPos The initial position of the car (in meters).
 * @param world Pointer to the world environment for boundary checking.
 */
Car::Car(Vector2 startPos, const World *world)
    : position(startPos), velocity{0, 0}, acceleration{0, 0}, world(world), maxSpeed(15.0f), maxForce(40.0f) {
} // 15 m/s (~54 km/h), 40 m/s^2 force

/**
 * @brief Updates the car's state based on time elapsed, without considering neighbors.
 *
 * @param dt Time elapsed since the last update.
 */
void Car::update(double dt) { updateWithNeighbors(dt, nullptr); }

/**
 * @brief Updates the car's state, including steering, physics, and collision avoidance.
 *
 * @param dt Time elapsed since the last update.
 * @param cars A pointer to the list of all cars in the scene, used for collision avoidance (can be nullptr).
 */
void Car::updateWithNeighbors(double dt, const std::vector<std::unique_ptr<Car>> *cars) {
  // --- Waypoint Logic: Generate a new random waypoint if none exist ---
  if (waypoints.empty()) {
    if (world) {
      // Random target in meters
      float dist = (float)GetRandomValue(15, 25); // 15-25 meters away
      float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
      Vector2 offset = {cosf(angle) * dist, sinf(angle) * dist};
      Vector2 nextPoint = Vector2Add(position, offset);

      // Clamp waypoint to stay within world bounds (with a 2.5m margin)
      if (nextPoint.x < 2.5f)
        nextPoint.x = 2.5f;
      if (nextPoint.y < 2.5f)
        nextPoint.y = 2.5f;
      if (nextPoint.x > world->getWidth() - 2.5f)
        nextPoint.x = world->getWidth() - 2.5f;
      if (nextPoint.y > world->getHeight() - 2.5f)
        nextPoint.y = world->getHeight() - 2.5f;

      addWaypoint(nextPoint);
    }
  }

  // --- Steering Behavior (Seek) ---
  if (!waypoints.empty()) {
    Vector2 target = waypoints.front();
    seek(target);

    // If close enough to the target (2.5m), move to the next waypoint
    if (Vector2Distance(position, target) < 2.5f) {
      waypoints.pop_front();
    }
  } else {
    // Apply friction/drag when no target is set
    velocity = Vector2Scale(velocity, 0.95f);
  }

  // --- Collision Avoidance (Braking and Separation) ---
  if (cars) {
    for (const auto &other : *cars) {
      if (other.get() == this)
        continue;

      float dist = Vector2Distance(position, other->getPosition());

      // If a car is within the detection range (3.5m), apply avoidance forces
      if (dist < 3.5f) {
        // 1. Apply Braking (Force opposite to current velocity)
        if (Vector2Length(velocity) > 0.5f) {
          Vector2 heading = Vector2Normalize(velocity);
          float brakingStrength = 30.0f;
          Vector2 brakingForce = Vector2Scale(heading, -brakingStrength);
          applyForce(brakingForce);
        }

        // 2. Apply Separation (Push the car away from the neighbor)
        Vector2 push = Vector2Subtract(position, other->getPosition());
        push = Vector2Normalize(push);

        // Strength of the push increases as distance decreases
        float pushStrength = 25.0f * (1.0f - (dist / 3.5f));
        applyForce(Vector2Scale(push, pushStrength));
      }
    }
  }

  // --- Physics Integration ---
  // Apply accumulated acceleration to velocity
  velocity = Vector2Add(velocity, Vector2Scale(acceleration, (float)dt));

  // Limit velocity to max speed
  if (Vector2Length(velocity) > maxSpeed) {
    velocity = Vector2Scale(Vector2Normalize(velocity), maxSpeed);
  }

  // Update Position
  position = Vector2Add(position, Vector2Scale(velocity, (float)dt));

  // Reset acceleration for the next frame
  acceleration = {0, 0};
}

/**
 * @brief Draws the car, its velocity vector, and its current waypoints.
 */
void Car::draw() {
  // Draw Waypoints and paths (Scaled by PPM)
  if (!waypoints.empty()) {
    for (size_t i = 0; i < waypoints.size(); ++i) {
      Vector2 wpScreen = Vector2Scale(waypoints[i], Config::PPM);
      // Radius: 0.25 meters
      DrawCircleV(wpScreen, 0.25f * Config::PPM, Fade(BLUE, 0.5f));
      if (i > 0) {
        Vector2 prevWpScreen = Vector2Scale(waypoints[i - 1], Config::PPM);
        DrawLineV(prevWpScreen, wpScreen, Fade(BLUE, 0.3f));
      } else {
        Vector2 posScreen = Vector2Scale(position, Config::PPM);
        DrawLineV(posScreen, wpScreen, Fade(BLUE, 0.3f));
      }
    }
  }

  // Draw Car rectangle (Scaled by PPM)
  // Car size: 4.5m x 1.8m
  float width = 4.5f * Config::PPM;
  float height = 1.8f * Config::PPM;

  float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;

  Vector2 posScreen = Vector2Scale(position, Config::PPM);

  Rectangle carRect = {posScreen.x, posScreen.y, width, height};
  // Origin is center of car
  DrawRectanglePro(carRect, {width / 2, height / 2}, rotation, RED);

  // Draw velocity vector (heading)
  Vector2 velEnd =
      Vector2Add(posScreen, Vector2Scale(velocity, Config::PPM * 0.5f)); // Scale velocity for visualization
  DrawLineV(posScreen, velEnd, GREEN);
}

/**
 * @brief Adds a point to the list of waypoints the car should follow.
 *
 * @param point The new waypoint coordinates (in meters).
 */
void Car::addWaypoint(Vector2 point) { waypoints.push_back(point); }

/**
 * @brief Clears all current waypoints.
 */
void Car::clearWaypoints() { waypoints.clear(); }

/**
 * @brief Applies a force vector to the car, accumulating in the acceleration vector.
 *
 * @param force The force vector to apply.
 */
void Car::applyForce(Vector2 force) { acceleration = Vector2Add(acceleration, force); }

/**
 * @brief Calculates the steering force required to move towards a target position (Seek behavior).
 *
 * @param target The target position to seek.
 */
void Car::seek(Vector2 target) {
  Vector2 desired = Vector2Subtract(target, position);
  desired = Vector2Normalize(desired);
  desired = Vector2Scale(desired, maxSpeed);

  Vector2 steer = Vector2Subtract(desired, velocity);

  // Limit the steering force to maxForce
  if (Vector2Length(steer) > maxForce) {
    steer = Vector2Scale(Vector2Normalize(steer), maxForce);
  }

  applyForce(steer);
}
