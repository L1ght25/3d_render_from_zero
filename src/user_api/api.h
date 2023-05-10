#pragma once
#include "../geometric_primitives/4d_primitives.h"
#include "../rendering/RenderContext.h"
#include "../rendering/bitmap/bitmap.h"
#include "../camera/Camera.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <exception>
#include <iostream>
#include <random>
#include <string_view>
#include <unistd.h>

namespace api {
class View {
  public:
    using Matrix4d = geometry::Matrix4d;

    View(std::string_view model_path, std::string_view texture_path,
        int width = constants::DEFAULT_WIDTH, int height = constants::DEFAULT_HEIGHT);

    void Execute(bool automode = true);

  private:

    std::pair<double, double> handle_mouse_rotation(double x, double y);

    geometry::Matrix4d get_transform(double rotate_state);

    geometry::Matrix4d get_transform(double rotate_state_x, double rotate_state_y);

    void image_to_bitmap(rendering::Bitmap& my_texture, const sf::Image& text);

    void render(double rotate_state);

    void render(double rotate_state_x, double rotate_state_y);

    sf::RenderWindow main_window_;
    sf::Texture screen_;

    rendering::Bitmap texture_;
    rendering::RenderContext main_renderer_;

    load::Object3d model_;

    Camera camera_;
};
}  // namespace api
