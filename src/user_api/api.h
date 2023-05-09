#pragma once
#include "../geometric_primitives/4d_primitives.h"
#include "../rendering/RenderContext.h"
#include "../rendering/bitmap/bitmap.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <exception>
#include <iostream>
#include <random>
#include <string_view>
#include <unistd.h>

namespace constants {
    inline constexpr int DEFAULT_WIDTH = 1000;
    inline constexpr int DEFAULT_HEIGHT = 1000;
    inline constexpr double MOVE_SCALE = 2;
    inline constexpr double ROTATE_SCALE = 80;
}

inline constexpr double radToDeg(double angleInRadians) {
    return angleInRadians * 180.0 / M_PI;
}

namespace api {
class View {
  public:
    using Matrix4d = geometry::Matrix4d;

    View(std::string_view model_path, std::string_view texture_path,
        int width = constants::DEFAULT_WIDTH, int height = constants::DEFAULT_HEIGHT);

    void Execute(bool automode = true);

  private:
    void handle_movement(double delta);

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
    geometry::Matrix4d projection_translation_;
};
}  // namespace api
