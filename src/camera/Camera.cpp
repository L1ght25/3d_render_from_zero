#include "Camera.h"
#include <cmath>

api::Camera::Camera(double x_pos, double y_pos, double z_pos, double angle, int width, int height, double z_near, double z_far) :
    projection_(Matrix4d().InitPerspective(radToDeg(angle), (double)width / (double)height, z_near, z_far)),
    camera_pos_(x_pos, y_pos, z_pos, 0), camera_direction_(0, 0, -1, 0), default_camera_direction_(camera_direction_),
    camera_up_(0, 1, 0, 0), yaw_(0), pitch_(0),
    translation_(geometry::Matrix4d().InitTranslationOperator(x_pos, y_pos, z_pos)) {
    rotation_.InitIdentityOperator();
}

void api::Camera::handle_movement(double delta) {
    geometry::Matrix4d movement;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        return;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        Vector4d move = camera_direction_ * delta * constants::MOVE_SCALE;
        camera_pos_ += move;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        Vector4d move = camera_direction_ * delta * constants::MOVE_SCALE;
        camera_pos_ -= move;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        Vector4d move = camera_direction_.CrossProduct(camera_up_).Normalize() * delta * constants::MOVE_SCALE;
        camera_pos_ -= move;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        Vector4d move = camera_direction_.CrossProduct(camera_up_).Normalize() * delta * constants::MOVE_SCALE;
        camera_pos_ += move;
    } else {
        return;
    }
    translation_ = movement.InitTranslationOperator(camera_pos_.GetX(), camera_pos_.GetY(), camera_pos_.GetZ());
}

void api::Camera::handle_rotation() {
    static bool is_first = true;
    if (is_first) {
        mouse_prev_x_ = sf::Mouse::getPosition().x;
        mouse_prev_y_ = sf::Mouse::getPosition().y;
        is_first = false;
    }
    double delta_x = (sf::Mouse::getPosition().x - mouse_prev_x_) / constants::ROTATE_SCALE;
    double delta_y = (sf::Mouse::getPosition().y - mouse_prev_y_) / constants::ROTATE_SCALE;
    yaw_ += delta_x;
    pitch_ += delta_y;
    if (pitch_ > M_PI_2) {
        pitch_ = M_PI_2;
    } else if (pitch_ < -M_PI_2) {
        pitch_ = -M_PI_2;
    }
    rotation_x_.InitRotation(1, 0, 0, pitch_);
    rotation_y_.InitRotation(0, 1, 0, yaw_);
    rotation_ = rotation_x_ * rotation_y_;

    camera_direction_ = rotation_y_ * rotation_x_ * default_camera_direction_;
    camera_direction_[0] *= -1;  // because of inverse value of z :))
    camera_direction_[1] *= -1;
    camera_direction_.Normalize();

    mouse_prev_x_ = sf::Mouse::getPosition().x;
    mouse_prev_y_ = sf::Mouse::getPosition().y;
}

void api::Camera::Update(double delta) {
    handle_rotation();
    handle_movement(delta);
}

geometry::Matrix4d api::Camera::GetTransform() const {
    return projection_ * rotation_ * translation_;
}