#pragma once
#include <cmath>

namespace constants {
inline constexpr int default_width = 1000;
inline constexpr int default_height = 1000;

inline constexpr double move_scale = 2;
inline constexpr double rotate_scale = 160;

inline constexpr int default_grey = 50;
inline constexpr int max_light = 255;

inline constexpr double default_projection_angle = 90;
inline constexpr double default_z_near = 0.1;
inline constexpr double default_z_far = 1000;

inline constexpr double default_camera_pos_z = 3;

inline constexpr double light_scale = 0.6;

inline constexpr double default_model_x = 0;
inline constexpr double default_model_y = 0;
inline constexpr double default_model_z = 0;

inline constexpr double radToDeg(double angleInRadians) {
    return angleInRadians * 180.0 / M_PI;
}
}  // namespace constants
