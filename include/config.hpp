#pragma once

namespace Config {
constexpr const char *WINDOW_TITLE = "ParkLogic"; ///< Window Title

// Logical Resolution (Render Texture Size)
constexpr int LOGICAL_WIDTH = 1280; ///< Internal resolution width
constexpr int LOGICAL_HEIGHT = 720; ///< Internal resolution height
constexpr int PIXELS_PER_ART_PIXEL = 3; ///< Resolution pixels per art pixel
constexpr int ART_PIXELS_PER_METER = 7; ///< Art pixels per meter
constexpr float PPM = static_cast<float>(PIXELS_PER_ART_PIXEL * ART_PIXELS_PER_METER);        // Pixels Per Meter (21.0f)

// Physical Window Start Size
constexpr int INITIAL_WINDOW_WIDTH = 1280; ///< Initial window width
constexpr int INITIAL_WINDOW_HEIGHT = 720; ///< Initial window height

constexpr int TICK_RATE = 60;                                             ///< Fixed update rate (ticks per second)
constexpr double FIXED_DELTA_TIME = 1.0 / static_cast<double>(TICK_RATE); ///< Time per tick

constexpr int TARGET_FPS = 60;       ///< Target frames per second
constexpr bool VSYNC_ENABLED = true; ///< Vertical sync flag
} // namespace Config
