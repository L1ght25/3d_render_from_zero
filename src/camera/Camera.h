#pragma once
#include "../constants/constants.h"
#include "../geometric_primitives/4d_primitives.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <unordered_map>

namespace api {
enum CameraEvent {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  NOTHING
};

class Camera {
  public:
    using Vector4d = geometry::Vector4d;
    using Matrix4d = geometry::Matrix4d;

    Camera(double x_pos, double y_pos, double z_pos, double angle, int width, int height, double z_near, double z_far);

    void UpdateMovement(double delta, CameraEvent key);

    void UpdateRotation(double delta_yaw, double delta_pitch);

    Matrix4d GetTransform() const;

  private:
    std::unordered_map<CameraEvent, std::function<void(double)>> movement_events_;

    Vector4d camera_direction_;
    Vector4d camera_pos_;

    static const Vector4d default_camera_direction_;
    static const Vector4d default_camera_up_;

    Matrix4d projection_;
    Matrix4d rotation_;
    Matrix4d translation_;
};
}  // namespace api
