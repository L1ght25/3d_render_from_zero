#include "Camera.h"
#include <cmath>

const geometry::Vector4d api::Camera::default_camera_direction_ = geometry::Vector4d(0, 0, -1, 0);
const geometry::Vector4d api::Camera::default_camera_up_ = geometry::Vector4d(0, 1, 0, 0);

api::Camera::Camera(double x_pos, double y_pos, double z_pos, double angle, int width, int height, double z_near, double z_far)
    : projection_(Matrix4d().InitPerspective(constants::radToDeg(angle), (double)width / (double)height, z_near, z_far)),
      camera_pos_(x_pos, y_pos, z_pos, 0), camera_direction_(default_camera_direction_),
      translation_(geometry::Matrix4d::InitTranslationOperator(x_pos, y_pos, z_pos)), rotation_(geometry::Matrix4d::InitIdentityOperator()) {
        movement_events_[CameraEvent::FORWARD] = [this] (double delta) -> void {
            Vector4d move = camera_direction_ * delta * constants::move_scale;
            camera_pos_ += move;
        };
        movement_events_[CameraEvent::BACKWARD] = [this] (double delta) -> void {
            Vector4d move = camera_direction_ * delta * constants::move_scale;
            camera_pos_ -= move;
        };
        movement_events_[CameraEvent::LEFT] = [this] (double delta) -> void {
            Vector4d move = camera_direction_.CrossProduct(default_camera_up_).Normalize() * delta * constants::move_scale;
            camera_pos_ -= move;
        };
        movement_events_[CameraEvent::RIGHT] = [this] (double delta) -> void {
            Vector4d move = camera_direction_.CrossProduct(default_camera_up_).Normalize() * delta * constants::move_scale;
            camera_pos_ += move;
        };
}

void api::Camera::UpdateRotation(double delta_yaw, double delta_pitch) {
    static double yaw = 0;
    static double pitch = 0;

    yaw += delta_yaw;
    pitch += delta_pitch;
    if (pitch > M_PI_2) {
        pitch = M_PI_2;
    } else if (pitch < -M_PI_2) {
        pitch = -M_PI_2;
    }
    auto rotation_x_ = Matrix4d::InitRotation(1, 0, 0, pitch);
    auto rotation_y_ = Matrix4d::InitRotation(0, 1, 0, yaw);

    rotation_ = rotation_x_ * rotation_y_;

    camera_direction_ = rotation_y_ * rotation_x_ * default_camera_direction_;
    camera_direction_[0] *= -1;  // because of inverse value of z :))
    camera_direction_[1] *= -1;
    camera_direction_.Normalize();
}

void api::Camera::UpdateMovement(double delta, CameraEvent key) {
    movement_events_[key](delta);
    translation_ = Matrix4d::InitTranslationOperator(camera_pos_.GetX(), camera_pos_.GetY(), camera_pos_.GetZ());
}

geometry::Matrix4d api::Camera::GetTransform() const {
    return projection_ * rotation_ * translation_;
}
