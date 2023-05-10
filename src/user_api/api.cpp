#include "api.h"

api::View::View(int width, int height)
    : main_window_(sf::VideoMode(width, height), "3d-rendered video"), main_renderer_(width, height),
    camera_(0, 0, 3, 90, width, height, 0.1, 1000) {
    screen_.create(width, height);
}

void api::View::LoadModel(std::string_view model_path, std::string_view texture_path, bool automode) {
    
    models_.emplace_back(model_path, automode);

    sf::Image text;
    text.loadFromFile(texture_path.data());
    texures_.emplace_back(text.getSize().x, text.getSize().y);
    image_to_bitmap(texures_.back(), text);
}

void api::View::Execute() {
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

geometry::Matrix4d api::View::get_transform(double rotate_state) {
    return camera_.GetTransform() * geometry::Matrix4d().InitRotation(0, rotate_state, rotate_state);
}

geometry::Matrix4d api::View::get_transform(double rotate_state_x, double rotate_state_y) {
    return camera_.GetTransform() * geometry::Matrix4d().InitRotation(rotate_state_x, rotate_state_y, 0);
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
    auto auto_transform = get_transform(rotate_state);
    main_renderer_.Fill(0);
    main_renderer_.ClearZBuffer();

    for (size_t i = 0; i < models_.size(); ++i) {
        main_renderer_.DrawModel(models_[i], models_[i].IsAutoTransformed() ? auto_transform : camera_.GetTransform(), texures_[i]);
    }
}
