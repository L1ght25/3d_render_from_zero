#include "api.h"

api::View::View(std::string_view model_path, std::string_view texture_path, int width, int height)
    : main_window_(sf::VideoMode(width, height), "3d-rendered video"), main_renderer_(width, height), model_(model_path), texture_(width, height) {
    sf::Image text;
    text.loadFromFile(texture_path.data());
    image_to_bitmap(texture_, text);
    auto projection = Matrix4d().InitPerspective(radToDeg(double(90)), (double)width / (double)height, 0.1, 1000.0);
    auto translation = Matrix4d().InitTranslationOperator(0, 0, 3);
    projection_translation_ = projection * translation;
    screen_.create(width, height);
}

void api::View::Execute(bool automode) {
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

        handle_movement(delta.count());
        if (automode) {
            rotate_state += delta.count();
            render(rotate_state);
        } else {
            std::tie(curr_x, curr_y) = handle_mouse_rotation(curr_x, curr_y);
            render(curr_y / constants::ROTATE_SCALE, curr_x / constants::ROTATE_SCALE);
        }

        prev_time = curr_time;

        screen_.update(main_renderer_.GetPointerToPixels());

        main_window_.draw(sprite);
        main_window_.display();
    }
}

void api::View::handle_movement(double delta) {
    geometry::Matrix4d movement;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        movement.InitTranslationOperator(0, 0, -delta * constants::MOVE_SCALE);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        movement.InitTranslationOperator(0, 0, delta * constants::MOVE_SCALE);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        movement.InitTranslationOperator(-delta * constants::MOVE_SCALE, 0, 0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        movement.InitTranslationOperator(delta * constants::MOVE_SCALE, 0, 0);
    } else {
        return;
    }
    projection_translation_ *= movement;
}

std::pair<double, double> api::View::handle_mouse_rotation(double x, double y) {
    static double delta_x = 0, delta_y = 0;
    static bool is_set = false;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        auto pos = sf::Mouse::getPosition();
        if (!is_set) {
            delta_x = pos.x - x;
            delta_y = pos.y - y;
            is_set = true;
        }
        return {pos.x - delta_x, pos.y - delta_y};
    }
    delta_x = 0, delta_y = 0;
    is_set = false;
    return {x, y};
}

geometry::Matrix4d api::View::get_transform(double rotate_state) {
    return projection_translation_ * geometry::Matrix4d().InitRotation(0, rotate_state, rotate_state);
}

geometry::Matrix4d api::View::get_transform(double rotate_state_x, double rotate_state_y) {
    return projection_translation_ * geometry::Matrix4d().InitRotation(rotate_state_x, rotate_state_y, 0);
}

void api::View::image_to_bitmap(rendering::Bitmap& my_texture, const sf::Image& text) {
    for (int x = 0; x < text.getSize().x; ++x) {
        for (int y = 0; y < text.getSize().y; ++y) {
            sf::Color col(text.getPixel(x, y));
            my_texture.SetPixel(x, y, col.r, col.g, col.b, 255);
        }
    }
}

void api::View::render(double rotate_state) {
    auto transform = get_transform(rotate_state);
    main_renderer_.Fill(0);
    main_renderer_.ClearZBuffer();
    main_renderer_.DrawModel(model_, transform, texture_);
}

void api::View::render(double rotate_state_x, double rotate_state_y) {
    auto transform = get_transform(rotate_state_x, rotate_state_y);
    main_renderer_.Fill(0);
    main_renderer_.ClearZBuffer();
    main_renderer_.DrawModel(model_, transform, texture_);
}