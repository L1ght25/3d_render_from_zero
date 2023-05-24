#include "api.h"
#include "../constants/constants.h"
#include <chrono>

api::Application::Application(std::initializer_list<Object3d>models, int width, int height)
    : sfml_details_(width, height), main_renderer_(width, height),
      camera_(0, 0, constants::default_camera_pos_z, constants::default_projection_angle, width, height, constants::default_z_near,
              constants::default_z_far),
      models_(models) {
}

void api::Application::Execute() {

    double rotate_state = 0;

    while (sfml_details_.main_window_.isOpen()) {
        CameraEvent last_event = HandleCameraEventsOrClose();
        UpdateWorld(rotate_state, last_event);

        Image screen_view = main_renderer_.MakeImage(models_, camera_, rotate_state);

        sfml_details_.Draw(screen_view);
    }
}

api::CameraEvent api::Application::HandleCameraEventsOrClose() {
    sf::Event event;
    while (sfml_details_.main_window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            sfml_details_.main_window_.close();
            break;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        return CameraEvent::FORWARD;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        return CameraEvent::BACKWARD;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        return CameraEvent::LEFT;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        return CameraEvent::RIGHT;
    } else {
        return CameraEvent::NOTHING;
    }
}

void api::Application::UpdateWorld(double& rotate_state, CameraEvent event) {
    static double mouse_prev_x_ = sf::Mouse::getPosition().x;
    static double mouse_prev_y_ = sf::Mouse::getPosition().y;
    static auto prev_time = std::chrono::system_clock::now();

    sfml_details_.main_window_.clear();
    auto curr_time = std::chrono::system_clock::now();
    std::chrono::duration<double> delta = curr_time - prev_time;

    double delta_yaw = (sf::Mouse::getPosition().x - mouse_prev_x_) / constants::rotate_scale;
    double delta_pitch = (sf::Mouse::getPosition().y - mouse_prev_y_) / constants::rotate_scale;

    camera_.UpdateRotation(delta_yaw, delta_pitch);
    if (event != CameraEvent::NOTHING) {
        camera_.UpdateMovement(delta.count(), event);
    }
    rotate_state += delta.count();

    mouse_prev_x_ = sf::Mouse::getPosition().x;
    mouse_prev_y_ = sf::Mouse::getPosition().y;
    prev_time = curr_time;
}

api::Application::SfmlDrawConnection::SfmlDrawConnection(int width, int height) : main_window_(sf::VideoMode(width, height), "3d-rendered video") {
    screen_.create(width, height);
    sprite_ = sf::Sprite{screen_};
}

void api::Application::SfmlDrawConnection::Draw(Image image) {
    screen_.update(image);
    main_window_.draw(sprite_);
    main_window_.display();
}