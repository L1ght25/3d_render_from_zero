#pragma once
#include <SFML/Graphics.hpp>
#include "../geometric_primitives/4d_primitives.h"

namespace constants {
    inline constexpr int DEFAULT_WIDTH = 1000;
    inline constexpr int DEFAULT_HEIGHT = 1000;
    inline constexpr double MOVE_SCALE = 2;
    inline constexpr double ROTATE_SCALE = 160;
}

inline constexpr double radToDeg(double angleInRadians) {
    return angleInRadians * 180.0 / M_PI;
}

namespace api {
    class Camera {
        public:
        using Vector4d = geometry::Vector4d;
        using Matrix4d = geometry::Matrix4d;

        Camera(double x_pos, double y_pos, double z_pos, double angle, int width, int height, double z_near, double z_far);

        void Update(double delta);

        Matrix4d GetTransform() const;

        private:

        void handle_movement(double delta);

        void handle_rotation();
        
        Vector4d camera_direction_;
        Vector4d default_camera_direction_;
        Vector4d camera_pos_;
        Vector4d camera_up_;

        Matrix4d projection_;
        Matrix4d rotation_;
        Matrix4d translation_;

        Matrix4d rotation_x_;
        Matrix4d rotation_y_;

        double mouse_prev_x_;
        double mouse_prev_y_;

        double yaw_;
        double pitch_;
    };
}