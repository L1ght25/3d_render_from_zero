#pragma once
#include "../constants/constants.h"
#include "../geometric_primitives/4d_primitives.h"
#include <SFML/Graphics.hpp>

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
}  // namespace api