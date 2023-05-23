#pragma once
#include <cmath>

namespace constants {
inline constexpr int DEFAULT_WIDTH = 1000;
inline constexpr int DEFAULT_HEIGHT = 1000;

inline constexpr double MOVE_SCALE = 2;
inline constexpr double ROTATE_SCALE = 160;

inline constexpr int DEFAULT_GREY = 50;
inline constexpr int MAX_LIGHT = 255;

inline constexpr double DEFAULT_PROJECTION_ANGLE = 90;
inline constexpr double DEFAULT_Z_NEAR = 0.1;
inline constexpr double DEFAULT_Z_FAR = 1000;

inline constexpr double DEFAULT_CAMERA_POS_Z = 3;

inline constexpr double LIGHT_SCALE = 0.6;

inline constexpr double radToDeg(double angleInRadians) {
    return angleInRadians * 180.0 / M_PI;
}
}  // namespace constants
