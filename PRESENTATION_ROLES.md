# Project Presentation Division: ParkLogic

This document delineates the responsibilities for the "ParkLogic" project presentation, divided among five presenters. The division prioritizes a heavy consolidation of core infrastructure into the **Engine** role, while distributing the specific domain logic (World, Traffic, AI, UI) into four distinct, loosely coupled domains.

---

## 1. The Core Engine Architect
**Scope**: The foundational framework, systems integration, and main execution lifecycle. This is the largest and most complex division, encompassing all "non-gameplay" systems and the container for the game itself.

### Primary Responsibilities
*   **Application Lifecycle**: Entry point, window creation, and the clean shutdown sequence.
*   **Scene Management**: The generic state machine that switches between Menu and Game.
*   **The Simulation Container**: Managing the `GameScene` where all other systems coexist.
*   **Input & Camera**: Abstraction of hardware input and management of the viewport (zoom/pan/clamp).
*   **Event Architecture**: Maintenance of the type-safe `EventBus` used for inter-system communication.
*   **Entity Management**: The generic container logic for managing object lifecycles (`EntityManager`).
*   **Resource Management**: Loading and caching of assets (Textures).

### Detailed File Manifest
*   **`src/main.cpp`**: Entry point. Exception handling try/catch block.
*   **`src/core/Application.cpp` / `.hpp`**:
    *   **Key Code**: `Application::run()`. Initializes Subsystems (`Window`, `EventBus`, `SceneManager`).
*   **`src/scenes/SceneManager.cpp` / `.hpp`**:
    *   **Key Code**: `SceneManager::changeScene()`. Swaps unique pointers to change the active state.
*   **`src/scenes/GameScene.cpp` / `.hpp`**:
    *   **Key Code**: `GameScene::update()`. The "Main Stage" that calls update on Traffic, AI, and UI systems.
*   **`src/core/Window.cpp` / `.hpp`**:
    *   **Key Code**: `Window::updateDimensions()`. Calculates the aspect-ratio preserving scale factor for logical resolution.
*   **`src/core/GameLoop.cpp` / `.hpp`**:
    *   **Key Code**: `GameLoop::run()`. Implements the "Fix Your Timestep" algorithm using an `accumulator` to ensure physics runs at steady 60hz.
*   **`src/core/EventBus.hpp`**:
    *   **Key Code**: `EventBus::subscribe<T>()`. Uses C++ Templates and `std::type_index` to map event types to callback lists.
*   **`src/core/EntityManager.cpp` / `.hpp`**:
    *   **Key Code**: `EntityManager::update()`. Iterates through generic collections (`cars`, `modules`) to trigger polymorphic updates.
*   **`src/systems/CameraSystem.cpp` / `.hpp`**:
    *   **Key Code**: `CameraSystem::update()`. Applies multiplicative zoom factors and clamps `target` coordinates to `worldBounds`.
*   **`src/input/InputSystem.cpp` / `.hpp`**:
    *   **Key Code**: `InputSystem::update()`. Polls Raylib's `GetKeyPressed` and publishes `KeyPressedEvent` / `MouseMovedEvent`.

---

## 2. The World Infrastructure Engineer
**Scope**: The procedural generation algorithms and the physical definition of the map environment.

### Primary Responsibilities
*   **Procedural Algorithm**: The logic that creates a valid, non-overlapping map layout based on configuration.
*   **Module Definitions**: The hierarchy of `Road` and `Facility` classes.
*   **Geometry & Alignment**: Defining `AttachmentPoint`s and ensuring adjacent modules snap together correctly.
*   **World Rendering**: Drawing the background grid and tiled terrain.

### Detailed File Manifest
*   **`src/entities/map/WorldGenerator.cpp` / `.hpp`**:
    *   **Key Code**: `WorldGenerator::generate()`.
        *   *Planning Phase*: Creates a purely logical vector of `PlannedUnit` structs (Road + Top Facility + Bottom Facility).
        *   *Placement Phase*: Iterates the plan, calculates linear X-coordinates, and handles collision/padding by inserting extra roads.
*   **`src/entities/map/Modules.cpp` / `.hpp`**:
    *   **Key Code**: `Module` base class. Defines `attachmentPoints` (Vector2 locations relative to top-left).
    *   **Key Code**: `SmallParking`, `LargeChargingStation`. Specific implementations that define spot layouts (arrays of coordinates).
*   **`src/entities/map/World.cpp` / `.hpp`**:
    *   **Key Code**: `World::draw()`. Renders the background tiling and the debug grid. Defines the physical boundaries of the simulation.

---

## 3. The Traffic Simulation Director
**Scope**: The high-level rules, spawning logic, and global state management of the simulation.

### Primary Responsibilities
*   **Spawning Logic**: Deterministic and randomized timers for introducing cars into the world.
*   **Destination Arbitration**: The algorithm that selects the "best" parking spot for a car based on availability, price, and priority.
*   **Game Rules**: Management of simulation speed and auto-spawn levels.
*   **Event Definitions**: Defining the dictionary of events that other systems use to communicate.

### Detailed File Manifest
*   **`src/systems/TrafficSystem.cpp` / `.hpp`**:
    *   **Key Code**: `TrafficSystem::update()`. Manages the `spawnTimer`.
    *   **Key Code**: `TrafficSystem::findBestSpotForCar()`. Iterates through all modules, checks `SpotState::FREE`, and scores them based on the car's `Priority` (Price vs. Distance).
    *   **Key Code**: `TrafficSystem::handleCarExit()`. Detects when a car has finished its lifecycle and publishes `CarDespawnEvent`.
*   **`include/config.hpp`**:
    *   **Key Code**: `namespace Spawner`. Defines spawn rate arrays and `BATTERY_THRESHOLDS`.
*   **`include/events/GameEvents.hpp`**:
    *   **Key Code**: Structs like `SpawnCarEvent`, `CarFinishedParkingEvent`, `SimulationSpeedChangedEvent`.

---

## 4. The Autonomous Agent Engineer
**Scope**: The mathematical logic, physics, and pathfinding for individual entities.

### Primary Responsibilities
*   **Path Planning**: Geometric calculation of curves and linear segments to create smooth trajectories.
*   **Steering & Physics**: Application of forces (Velocity, Acceleration) and steering behaviors (Seek, Arrival).
*   **State Machine**: Handling the complex transitions of a car (Idle -> Driving -> Parking -> Exiting).
*   **Collision Avoidance**: Ray-casting or distance-checking against other cars to prevent overlaps.

### Detailed File Manifest
*   **`src/systems/PathPlanner.cpp` / `.hpp`**:
    *   **Key Code**: `PathPlanner::GeneratePath()`. Constructs a `std::vector<Waypoint>` by stitching together:
        1.  Approach Path (Highway).
        2.  Access Path (Road T-Junction entry).
        3.  Maneuver Path (Alignment).
        4.  Parking Path (Final spot).
    *   **Key Code**: `CalculateRoadEntry()`. Uses complex offsets (`LANE_OFFSET_UP`, `LANE_OFFSET_DOWN`) to ensure cars enter roads in the correct lane.
*   **`src/entities/Car.cpp` / `.hpp`**:
    *   **Key Code**: `Car::updateWithNeighbors()`. The central AI loop.
    *   **Key Code**: `Car::applySteering()`. Implements Reynolds' steering behavior: `Steering = DesiredVelocity - CurrentVelocity`.
    *   **Key Code**: `Car::followPath()`. Checks distance to current `targetWaypoint` and advances the index when within `tolerance`.

---

## 5. The Interface Designer (UX, Menu & Config)
**Scope**: The visualization layer, user interaction, main menu flow, and simulation configuration.

### Primary Responsibilities
*   **Main Menu**: The entry point for the user (`MainMenuScene`).
*   **Configuration**: Managing `MapConfig` values (modules counts) selected by the user.
*   **HUD Management**: Composition of the on-screen display during the game (`GameHUD`).
*   **Data Visualization**: The `DashboardOverlay` which acts as a real-time monitor for simulation stats.
*   **Interaction Logic**: The `UIButton` component and hover/click state management.

### Detailed File Manifest
*   **`src/scenes/MainMenuScene.cpp` / `.hpp`**:
    *   **Key Code**: `MainMenuScene::draw()`. Renders the configuration sliders and "Start Simulation" button.
    *   **Key Code**: Building the `MapConfig` struct to pass to the `GenerateWorldEvent`.
*   **`include/config.hpp`**:
    *   **Key Code**: `struct MapConfig`. The data structure that holds the user's simulation choices.
*   **`src/ui/GameHUD.cpp` / `.hpp`**:
    *   **Key Code**: Constructor logic. Instantiates the `DashboardOverlay` and creates the "Spawn Car" / "Speed" buttons.
*   **`src/ui/DashboardOverlay.cpp` / `.hpp`**:
    *   **Key Code**: `DashboardOverlay::draw()`. Uses `std::format` to render live data like "Occupancy: 45%" or "Car State: PARKING".
    *   **Key Code**: Subscription to `EntitySelectedEvent`. Updates the internal `currentSelection` state only when the user clicks a car, demonstrating efficient event-driven UI updates.
*   **`src/ui/UIButton.cpp` / `.hpp`**:
    *   **Key Code**: `UIButton` constructor. Subscribes to `MouseMovedEvent` to handle `isHovered` state changes and `MouseClickEvent` for triggering callbacks.
*   **`src/ui/UIManager.hpp`**:
    *   **Key Code**: Simple container to batch update/draw calls for all UI elements.
