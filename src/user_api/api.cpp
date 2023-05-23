#include "api.h"
#include "../constants/constants.h"

api::Application::Application(std::initializer_list<Object3d>models, int width, int height)
    : main_window_(sf::VideoMode(width, height), "3d-rendered video"), main_renderer_(width, height),
      camera_(0, 0, constants::default_camera_pos_z, constants::default_projection_angle, width, height, constants::default_z_near,
              constants::default_z_far),
      models_(models) {
    screen_.create(width, height);
}

void api::Application::Execute() {
    sf::Sprite sprite(screen_);
    screen_.update(main_renderer_.GetPointerToPixels());

    double rotate_state = 0;
    double curr_x = 0, curr_y = 0;
    auto prev_time = std::chrono::system_clock::now();

    while (main_window_.isOpen()) {
        sf::Event event;
        while (main_window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                main_window_.close();
                break;
            }
        }
        main_window_.clear();

        auto curr_time = std::chrono::system_clock::now();
        std::chrono::duration<double> delta = curr_time - prev_time;

        camera_.Update(delta.count());
        rotate_state += delta.count();
        render(rotate_state);

        prev_time = curr_time;

        screen_.update(main_renderer_.GetPointerToPixels());

        main_window_.draw(sprite);
        main_window_.display();
    }
}

geometry::Matrix4d api::Application::get_object_transform(double rotate_state) {
    return geometry::Matrix4d().InitRotation(0, rotate_state, rotate_state);
}

geometry::Matrix4d api::Application::get_object_transform(double rotate_state_x, double rotate_state_y) {
    return geometry::Matrix4d().InitRotation(rotate_state_x, rotate_state_y, 0);
}

void api::Application::render(double rotate_state) {
    static auto identity = Matrix4d().InitIdentityOperator();
    auto all_objects_transform = get_object_transform(rotate_state);
    main_renderer_.Fill(constants::default_grey);
    main_renderer_.ClearZBuffer();

    for (size_t i = 0; i < models_.size(); ++i) {
        auto object_transform = models_[i].IsAutoTransformed() ? all_objects_transform : identity;
        main_renderer_.DrawModel(models_[i], camera_.GetTransform() * object_transform, object_transform);
    }
}
