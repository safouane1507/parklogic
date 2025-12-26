#include "entities/Car.hpp"
#include "entities/map/World.hpp"
#include "raymath.h"
#include <memory>
#include <vector>

#include "config.hpp"
#include "core/AssetManager.hpp"

/**
 * @file Car.cpp
 * @brief Implementation of the Car entity.
 *
 * This class manages vehicle physics, steering behaviors, autonomous navigation
 * via waypoints, and state transitions (Driving, Parking, Charging).
 */

/**
 * @brief Constructs a new Car object.
 *
 * @param startPos Initial position in meters.
 * @param world Pointer to the world environment for bounds and collision.
 * @param initialVelocity Initial velocity vector.
 * @param type The propulsion type (Combustion or Electric).
 */
Car::Car(Vector2 startPos, const World * /*world*/, Vector2 initialVelocity, CarType type)
    : position(startPos), velocity(initialVelocity), acceleration{0, 0}, maxSpeed(15.0f), maxForce(60.0f), type(type) {

  // Select a random visual variant (1-3) based on vehicle type
  int variant = GetRandomValue(1, 3);
  if (type == CarType::COMBUSTION) {
    textureName = "car1" + std::to_string(variant);
    batteryLevel = 0.0f;
  } else {
    textureName = "car2" + std::to_string(variant);
    batteryLevel = (float)GetRandomValue(10, 90); // Initialize with random charge
  }

  // Set initial heading based on starting velocity
  if (Vector2Length(velocity) > 0.1f) {
    currentRotation = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
  }
}

/**
 * @brief Increases the battery level for electric vehicles.
 * @param amount Percentage points to add.
 */
void Car::charge(float amount) {
  if (type == CarType::ELECTRIC) {
    batteryLevel += amount;
    if (batteryLevel > 100.0f)
      batteryLevel = 100.0f;
  }
}

/**
 * @brief Assigns the car to a specific parking location.
 */
void Car::setParkingContext(const Module *fac, const Spot &spot, int spotIndex) {
  parkedFacility = fac;
  parkedSpot = spot;
  parkedSpotIndex = spotIndex;
}

/**
 * @brief Standard update override.
 * Calls updateWithNeighbors with no neighbor context.
 */
void Car::update(double dt) { updateWithNeighbors(dt, nullptr); }

/**
 * @brief Core AI and Physics update loop.
 *
 * Logic flow:
 * 1. State Management (Handle static states like PARKED).
 * 2. Path Following (Calculate steering toward current waypoint).
 * 3. Collision Avoidance (Apply braking/repulsion based on nearby cars).
 * 4. Physics Integration (Apply forces to velocity and position).
 * 5. Visual Rotation (Smoothly lerp sprite rotation toward heading).
 *
 * @param dt Delta time in seconds.
 * @param cars Pointer to a vector of other cars for spatial awareness.
 */
void Car::updateWithNeighbors(double dt, const std::vector<std::unique_ptr<Car>> *cars) {

  // 1. Handle Static States
  if (state == CarState::PARKED) {
    parkingTimer -= (float)dt;
    return;
  }

  // 2. Path Following (Seek Logic)
  if (!waypoints.empty()) {
    Waypoint &currentWp = waypoints.front();
    seek(currentWp);

    // Check if waypoint reached (within tolerance)
    if (Vector2Distance(position, currentWp.position) < currentWp.tolerance) {
      if (waypoints.size() == 1) {
        // Transition to alignment/parking if this is the final waypoint
        if (currentWp.stopAtEnd && state == CarState::DRIVING) {
          velocity = {0, 0};
          acceleration = {0, 0};
          state = CarState::ALIGNING;
          targetRotation = currentWp.entryAngle;
        }
      }
      waypoints.pop_front();
    }
  } else {
    // Logic for cars currently parking (Aligning to the spot angle)
    if (state == CarState::ALIGNING) {
      float targetDeg = (targetRotation * RAD2DEG) + 90.0f;
      float rotSpeed = 120.0f;
      float diff = targetDeg - currentRotation;

      // Normalize angle difference to [-180, 180]
      while (diff > 180.0f)
        diff -= 360.0f;
      while (diff <= -180.0f)
        diff += 360.0f;

      if (fabs(diff) < 1.0f) {
        currentRotation = targetDeg;
        state = CarState::PARKED;
        parkingTimer =
            (float)GetRandomValue((int)(Config::PARKING_MIN_TIME * 10), (int)(Config::PARKING_MAX_TIME * 10)) / 10.0f;
      } else {
        float change = rotSpeed * (float)dt;
        if (change > fabs(diff))
          change = fabs(diff);
        currentRotation += (diff > 0) ? change : -change;
      }
      return;
    } else if (state == CarState::DRIVING) {
      // Apply friction/drag if no waypoints exist
      velocity = Vector2Scale(velocity, 0.95f);
    }
  }

  // 3. Collision Avoidance and "Creep" Logic
  if (cars && (state == CarState::DRIVING || state == CarState::EXITING)) {
    // Determine current heading vector
    Vector2 heading = (Vector2Length(velocity) > 0.1f) ? Vector2Normalize(velocity)
                                                       : Vector2{cosf((currentRotation - 90.0f) * DEG2RAD),
                                                                 sinf((currentRotation - 90.0f) * DEG2RAD)};
    Vector2 sideVec = {-heading.y, heading.x};

    float currentSpeed = Vector2Length(velocity);
    float lookAheadDist = 7.0f + (currentSpeed * 2.0f);
    float laneWidth = 1.8f;
    float criticalStopDist = 3.2f;

    for (const auto &other : *cars) {
      if (other.get() == this || other->state == CarState::PARKED)
        continue;

      Vector2 toOther = Vector2Subtract(other->getPosition(), position);
      float distSq = Vector2LengthSqr(toOther);

      if (distSq > lookAheadDist * lookAheadDist)
        continue;

      float dotForward = Vector2DotProduct(toOther, heading);
      float dotSide = Vector2DotProduct(toOther, sideVec);

      Vector2 otherHeading = (Vector2Length(other->velocity) > 0.1f) ? Vector2Normalize(other->velocity) : heading;
      float alignment = Vector2DotProduct(heading, otherHeading);

      // Detection Corridor: Check if 'other' is directly in front
      if (dotForward > 0 && dotForward < lookAheadDist && fabsf(dotSide) < laneWidth) {

        // Ignore oncoming traffic in adjacent lanes
        if (alignment < -0.5f && fabsf(dotSide) > 1.0f)
          continue;

        // A. Apply Braking Force proportional to proximity
        float proximity = 1.0f - (dotForward / lookAheadDist);
        float brakingForce = 45.0f * (proximity * proximity);

        // B. Critical Distance Damping
        if (dotForward < criticalStopDist) {
          brakingForce += 60.0f;
          // Only force-damp if moving; allows for low-speed "creeping"
          if (currentSpeed > 0.3f) {
            velocity = Vector2Scale(velocity, 0.85f);
          }
        }

        applyForce(Vector2Scale(heading, -brakingForce));

        // C. Deadlock Breaker: Lateral nudge if the car is stuck behind another
        if (currentSpeed < 0.5f) {
          float steerDir = (dotSide > 0) ? -1.0f : 1.0f;
          applyForce(Vector2Scale(sideVec, 40.0f * steerDir));
        }
      }

      // D. Lateral Separation (Repulsion from nearby neighbors)
      float dist = sqrtf(distSq);
      if (dist < 1.9f) {
        float pushStrength = 30.0f * (1.0f - (dist / 1.9f));
        Vector2 pushDir = Vector2Normalize(toOther);
        float lateralPush = Vector2DotProduct(pushDir, sideVec);
        applyForce(Vector2Scale(sideVec, lateralPush * -pushStrength));
      }
    }
  }

  // 4. Physics Integration
  if (state != CarState::PARKED && state != CarState::ALIGNING) {
    // Constant drag force
    applyForce(Vector2Scale(velocity, -0.05f));

    // Integrate acceleration into velocity
    velocity = Vector2Add(velocity, Vector2Scale(acceleration, (float)dt));

    // Clamp to max speed
    if (Vector2Length(velocity) > maxSpeed) {
      velocity = Vector2Scale(Vector2Normalize(velocity), maxSpeed);
    }

    // Stuck Prevention: Zero out micro-movements to prevent jitter
    if (Vector2Length(velocity) < 0.05f && Vector2Length(acceleration) < 2.0f) {
      velocity = {0, 0};
    }

    // Integrate velocity into position
    position = Vector2Add(position, Vector2Scale(velocity, (float)dt));

    // 5. Smooth Rotation: Interpolate current rotation toward velocity vector
    float speed = Vector2Length(velocity);
    if (speed > 0.1f) {
      float targetRot = atan2f(velocity.y, velocity.x) * RAD2DEG + 90.0f;
      float angleDiff = targetRot - currentRotation;
      while (angleDiff > 180)
        angleDiff -= 360;
      while (angleDiff < -180)
        angleDiff += 360;
      currentRotation += angleDiff * 0.12f;
    }
  }

  acceleration = {0, 0}; // Reset forces for next frame
}

/**
 * @brief Renders the car and optional debug information (paths/waypoints).
 * @param showPath If true, draws the car's planned trajectory.
 */
void Car::draw(bool showPath) {
  if (showPath && !waypoints.empty()) {
    for (size_t i = 0; i < waypoints.size(); ++i) {
      Vector2 wpPos = waypoints[i].position;
      DrawCircleV(wpPos, 0.25f, Fade(BLUE, 0.5f));
      if (i > 0) {
        DrawLineV(waypoints[i - 1].position, wpPos, Fade(BLUE, 0.3f));
      } else {
        DrawLineV(position, wpPos, Fade(BLUE, 0.3f));
      }
    }
  }

  Texture2D tex = AssetManager::Get().GetTexture(textureName);

  // Convert pixel dimensions to meters using config scaling
  float width = 17.0f / static_cast<float>(Config::ART_PIXELS_PER_METER);
  float height = 31.0f / static_cast<float>(Config::ART_PIXELS_PER_METER);

  Rectangle source = {0, 0, (float)tex.width, (float)tex.height};
  Rectangle dest = {position.x, position.y, width, height};
  Vector2 origin = {width / 2.0f, height / 2.0f};

  DrawTexturePro(tex, source, dest, origin, currentRotation, WHITE);
}

/**
 * @brief Appends a single waypoint to the path.
 */
void Car::addWaypoint(Waypoint wp) { waypoints.push_back(wp); }

/**
 * @brief Replaces current waypoints with a new path.
 */
void Car::setPath(const std::vector<Waypoint> &path) {
  waypoints.clear();
  for (const auto &wp : path) {
    waypoints.push_back(wp);
  }
}

/**
 * @brief Removes all waypoints from the path.
 */
void Car::clearWaypoints() { waypoints.clear(); }

/**
 * @brief Accumulates a force vector to be applied during the next physics update.
 */
void Car::applyForce(Vector2 force) { acceleration = Vector2Add(acceleration, force); }

/**
 * @brief Calculates steering force toward a target using Seek/Arrive behaviors.
 *
 * Includes logic for:
 * - Speed limits defined by waypoints.
 * - Turn slowdown (reducing speed based on angle difference).
 * - Arrival damping (slowing down as the final destination is reached).
 */
void Car::seek(const Waypoint &wp) {
  Vector2 target = wp.position;
  Vector2 desired = Vector2Subtract(target, position);
  float dist = Vector2Length(desired);
  desired = Vector2Normalize(desired);

  float currentAngle = atan2f(velocity.y, velocity.x);

  // 1. Base speed for this segment
  float limitSpeed = maxSpeed * wp.speedLimitFactor;
  float speed = limitSpeed;

  // 2. Turn Slowdown Logic
  float targetAngle = wp.entryAngle;
  float diff = currentAngle - targetAngle;
  while (diff <= -PI)
    diff += 2 * PI;
  while (diff > PI)
    diff -= 2 * PI;

  float angleDiff = fabs(diff);

  // If approaching a sharp turn, reduce target speed
  if (dist < Config::CarAI::TURN_SLOWDOWN_DIST) {
    if (angleDiff > Config::CarAI::TURN_SLOWDOWN_ANGLE) {
      float factor = (dist / Config::CarAI::TURN_SLOWDOWN_DIST);
      float turnMinSpeed = maxSpeed * Config::CarAI::TURN_MIN_SPEED_FACTOR;

      // Linearly interpolate speed toward the turn-safe minimum
      float flexibleSpeed = turnMinSpeed + (limitSpeed - turnMinSpeed) * factor;

      if (speed > flexibleSpeed) {
        speed = flexibleSpeed;
      }
    }
  }

  // 3. Arrival Logic (Stopping at the final waypoint)
  if (wp.stopAtEnd) {
    float stopRadius = 10.0f;
    if (dist < stopRadius) {
      float t = dist / stopRadius;
      float stopSpeed = maxSpeed * t;

      if (stopSpeed < speed) {
        speed = stopSpeed;
      }

      // Ensure minimum velocity to prevent stopping prematurely
      if (speed < 0.5f)
        speed = 0.5f;
    }
  }

  desired = Vector2Scale(desired, speed);
  Vector2 steer = Vector2Subtract(desired, velocity);

  // Clamp steering force to vehicle capabilities
  if (Vector2Length(steer) > maxForce) {
    steer = Vector2Scale(Vector2Normalize(steer), maxForce);
  }

  applyForce(steer);
}
